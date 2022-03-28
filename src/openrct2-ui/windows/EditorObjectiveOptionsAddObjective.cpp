/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Dropdown.h"
#include "../interface/Widget.h"
#include "../interface/Window.h"
#include "Window.h"

#include <algorithm>
#include <iterator>
#include <openrct2-ui/interface/Theme.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/ParkSetNameAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Font.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Park.h>

uint32_t originalGoalIndex;
ObjectiveGoalPtr goal;
ObjectiveGoalGroup* group;
GoalID goalID;
bool editMode;
bool deleteTrackPiecesMode;
std::vector<uint8_t> dropDownRides;
std::vector<uint16_t> dropDownRides2;
std::vector<uint16_t> dropDownPieces;
std::vector<rct_string_id> dropDownRideTypes;
std::optional<size_t> _highlightedIndex = 0;

constexpr size_t BUF_SIZE = 50;
static char textbox1Buffer[BUF_SIZE];
static char textbox2Buffer[BUF_SIZE];
static char textbox3Buffer[BUF_SIZE];
static char textbox4Buffer[BUF_SIZE];
static char textbox5Buffer[BUF_SIZE];
static char textbox6Buffer[BUF_SIZE];
static char textbox7Buffer[BUF_SIZE];
static char textbox8Buffer[BUF_SIZE];
static char textbox9Buffer[BUF_SIZE];
static char textbox10Buffer[BUF_SIZE];
static char textbox11Buffer[BUF_SIZE];
static char textbox12Buffer[BUF_SIZE];
static char textbox13Buffer[BUF_SIZE];
static char textbox14Buffer[BUF_SIZE];
static char textbox15Buffer[BUF_SIZE];
static char textbox16Buffer[BUF_SIZE];

static constexpr const int32_t WW = 250;

#pragma region Widgets

// clang-format off





enum {
    WIDX_ADD_OBJECTIVE_BACKGROUND,
    WIDX_ADD_OBJECTIVE_TITLE,
    WIDX_ADD_OBJECTIVE_CLOSE,
    WIDX_OKAY,
    WIDX_CANCEL,
    WIDX_DELETE,

    WIDX_AWARD = 6,
    WIDX_AWARD_DROPDOWN,
    WIDX_AWARD_ANYTIME,
        
    WIDX_RIDE_NUM_MIN = 6,   
    WIDX_RIDE_NUM_MAX,
    WIDX_RIDE_UNIQUE,
    WIDX_RIDE_FINISHED_RIDES,
    WIDX_RIDE_LENGTH_MIN,
    WIDX_RIDE_LENGTH_MAX,
    WIDX_RIDE_EXCITEMENT_MIN,
    WIDX_RIDE_EXCITEMENT_MAX,
    WIDX_RIDE_INTENSITY_MIN,
    WIDX_RIDE_INTENSITY_MAX,
    WIDX_RIDE_NAUSEA_MIN,
    WIDX_RIDE_NAUSEA_MAX,

    WIDX_VALUE_SET_AMOUNT = 6,
    WIDX_VALUE_SIGN,
    WIDX_VALUE_SIGN_DROPDOWN,
    WIDX_VALUE_RESTRICTION,
    WIDX_VALUE_WARNING_WEEKS,
    WIDX_VALUE_WARNING_WEEKS_INCREASE,
    WIDX_VALUE_WARNING_WEEKS_DECREASE,
    
    WIDX_NUM_AWARDS = 6,
    WIDX_NUM_AWARDS_INCREASE,
    WIDX_NUM_AWARDS_DECREASE,

    WIDX_BUILD_RIDE = 6,
    WIDX_BUILD_RIDE_DROPDOWN,
    WIDX_BUILD_RIDE_UNIQUE,
    WIDX_BUILD_RIDE_BEST,

    WIDX_BUILD_TRACK = 6, //keep the same as WIDX_BUILD_RIDE, same for the dropdown.
    WIDX_BUILD_TRACK_DROPDOWN, //keep same
    WIDX_BUILD_TRACK_UNIQUE, //keep same
    WIDX_BUILD_TRACK_DROPS_MIN,
    WIDX_BUILD_TRACK_DROPS_MAX,
    WIDX_BUILD_TRACK_HEIGHT_MIN,
    WIDX_BUILD_TRACK_HEIGHT_MAX,
    WIDX_BUILD_TRACK_LENGTH_MIN,
    WIDX_BUILD_TRACK_LENGTH_MAX,
    WIDX_BUILD_TRACK_EXCITEMENT_MIN,
    WIDX_BUILD_TRACK_EXCITEMENT_MAX,
    WIDX_BUILD_TRACK_INTENSITY_MIN,
    WIDX_BUILD_TRACK_INTENSITY_MAX,
    WIDX_BUILD_TRACK_NAUSEA_MIN,
    WIDX_BUILD_TRACK_NAUSEA_MAX,
    WIDX_BUILD_TRACK_SPEED_MIN,
    WIDX_BUILD_TRACK_SPEED_MAX,
    WIDX_BUILD_TRACK_INVERSIONS_MIN,
    WIDX_BUILD_TRACK_INVERSIONS_MAX,
    WIDX_BUILD_TRACK_SCROLL,
    WIDX_BUILD_TRACK_ADD_PIECE,
    WIDX_BUILD_TRACK_ADD_PIECE_DROPDOWN,
    WIDX_BUILD_TRACK_REMOVE_PIECES,

};

static rct_widget window_editor_objective_options_award_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, WW, 132),
    MakeWidget({30,  114}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({150,  114}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({113,  104}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL), /*Delete*/
    MakeWidget({ 105,  59}, {140,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,STR_SELECT_AWARD    ), /*Award dropdown*/
    MakeWidget({233, 60}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_AWARD), /*Award dropdown*/
    MakeWidget({232,  82}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_AWARD_ANYTIME),/*Any time*/
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_no_duplicate_rides_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, WW, 100),
    MakeWidget({30,  82}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({150,  82}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({113,  72}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL), /*Delete*/
    MakeWidget({ 105,  57}, {140,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,STR_SELECT_AWARD    ), /*Category dropdown*/
    MakeWidget({233, 58}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_AWARD), /*Category dropdown*/
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_coaster_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, 300, 353),
    MakeWidget({30,  335}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({200,  335}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({138,  325}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL ), /*Delete*/
    MakeWidget({ 195,  87}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MIN_RIDE_NUM), /*ride min num*/
    MakeWidget({ 195,  107}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MAX_RIDE_NUM), /*ride max num*/
    MakeWidget({282,  287}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_UNIQUE_RIDES),/*Unique*/
    MakeWidget({282,  307}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_FINISH_RIDES),/*Finish Existing Rides*/
    MakeWidget({ 195,  127}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MIN_TRACK_LENGTH), /*ride min length*/
    MakeWidget({ 195,  147}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MAX_TRACK_LENGTH), /*ride max length*/
    MakeWidget({ 195,  167}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MIN_EXCITEMENT_RATING), /*ride min excitement*/
    MakeWidget({ 195,  187}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MAX_EXCITEMENT_RATING), /*ride max excitement*/
    MakeWidget({ 195,  207}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MIN_INTENSITY_RATING), /*ride min intesity*/
    MakeWidget({ 195,  227}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MAX_INTENSITY_RATING), /*ride max intesity*/
    MakeWidget({ 195,  247}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MIN_NAUSEA_RATING), /*ride min nausea*/
    MakeWidget({ 195,  267}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MAX_NAUSEA_RATING), /*ride max nausea*/
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_simple_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, WW, 75),
    MakeWidget({30,  57}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({150,  57}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({113,  47}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL ), /*Delete*/
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_value_sign_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, WW, 175),
    MakeWidget({30,  157}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({150,  157}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({113,  147}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL ), /*Delete*/
    MakeWidget({175,  67}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_CHANGE ), /*Value*/
    MakeWidget({ 175,  87}, {70,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,STR_NONE     ), /*Sign dropdown*/
    MakeWidget({233, 88}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_NONE), /*Sign dropdown*/
    MakeWidget({232,  107}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_IS_RESTRICTION),/*Restriction*/
    MakeSpinnerWidgets({ 175,  127}, {70,  12}, WindowWidgetType::Spinner,   WindowColour::Secondary, STR_NONE, STR_LEEWAY_WEEKS), /*Leeway NB: 3 widgets*/
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_gentle_rides_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, 300, 173),
    MakeWidget({30,  155}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({200,  155}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({138,  145}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL ), /*Delete*/
    MakeWidget({ 195,  67}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MIN_RIDE_NUM), /*ride min num*/
    MakeWidget({ 195,  87}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MAX_RIDE_NUM), /*ride max num*/
    MakeWidget({282,  107}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_UNIQUE_RIDES),/*Unique*/
    MakeWidget({282,  127}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_FINISH_RIDES),/*Finish Existing Rides*/
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_num_awards_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, WW, 105),
    MakeWidget({30,  87}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({150,  87}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({113,  77}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL ), /*Delete*/
    MakeSpinnerWidgets({ 195,  47}, {50,  12}, WindowWidgetType::Spinner,   WindowColour::Secondary), /*num awards NB: 3 widgets*/
    WIDGETS_END,
};


static rct_widget window_editor_objective_options_park_size_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, WW, 135),
    MakeWidget({30,  117}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({150,  117}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({113,  107}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL ), /*Delete*/
    MakeWidget({175,  67}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_CHANGE ), /*Value*/
    MakeWidget({ 175,  87}, {70,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,STR_NONE     ), /*Sign dropdown*/
    MakeWidget({233, 88}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_NONE), /*Sign dropdown*/
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_research_ride_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, WW, 105),
    MakeWidget({30,  87}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({150,  87}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({113,  77}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL ), /*Delete*/
    MakeWidget({100,  57}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,STR_SELECT_RIDE_TYPE     ), /*Ride dropdown*/
    MakeWidget({233, 58}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_RIDE_TYPE), /*Ride dropdown*/
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_specific_ride_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, WW, 145),
    MakeWidget({30,  127}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({150,  127}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({113,  117}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL ), /*Delete*/
    MakeWidget({100,  57}, {145,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,STR_SELECT_RIDE_TYPE     ), /*Ride dropdown*/
    MakeWidget({233, 58}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_RIDE_TYPE), /*Ride dropdown*/
    MakeWidget({232,  77}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_MUST_BE_UNIQUE_RIDE),/*Unique*/
    MakeWidget({232,  97}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_COMPETION_GOAL),/*Build Best*/
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_tracked_ride_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, 600, 412),
    MakeWidget({30,  395}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({480,  395}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({263,  385}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL ), /*Delete*/
    MakeWidget({ 145,  100}, {150,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_NONE,STR_SELECT_RIDE_TYPE     ), /*Ride dropdown*/
    MakeWidget({283, 101}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_RIDE_TYPE), /*Ride dropdown*/
    MakeWidget({582,  100}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_MUST_BE_UNIQUE_RIDE),/*Unique*/
    MakeWidget({ 195,  120}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Min Drops*/
    MakeWidget({ 495,  120}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Max Drops*/
    MakeWidget({ 195,  140}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Min Height*/
    MakeWidget({ 495,  140}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Height*/
    MakeWidget({ 195,  160}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Min Length*/
    MakeWidget({ 495,  160}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Max Length*/
    MakeWidget({ 195,  180}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Min Excitement*/
    MakeWidget({ 495,  180}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Max Excitement*/
    MakeWidget({ 195,  200}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Min Intensity*/
    MakeWidget({ 495,  200}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Max Intensity*/
    MakeWidget({ 195,  220}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Min Nausea*/
    MakeWidget({ 495,  220}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Max Nausea*/
    MakeWidget({ 195,  240}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Min Speed*/
    MakeWidget({ 495,  240}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Max Speed*/
    MakeWidget({ 195,  260}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Min Inversions*/
    MakeWidget({ 495,  260}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary), /*Max Inversions*/
    MakeWidget        ({150, 280}, {445, 100}, WindowWidgetType::Scroll, WindowColour::Secondary, SCROLL_VERTICAL), //Scroll for required track pieces
    MakeWidget({ 5,  307}, {140,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_ADD_REQUIRED_TRACK_PIECE,STR_ADD_REQUIRED_TRACK_PIECE     ), //add track piece
    MakeWidget({ 5,  307}, {140,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_ADD_REQUIRED_TRACK_PIECE, STR_ADD_REQUIRED_TRACK_PIECE), 
    MakeWidget        ({121, 327}, { 24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_DEMOLISH,         STR_REMOVE_REQUIRED_TRACK_PIECE), //Remove trackpieces
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_thrill_ride_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, 300, 153),
    MakeWidget({30,  135}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({200,  135}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({138,  125}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL ), /*Delete*/
    MakeWidget({ 195,  67}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MIN_RIDE_NUM), /*ride min num*/
    MakeWidget({ 195,  87}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MAX_RIDE_NUM), /*ride max num*/
    MakeWidget({282,  107}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_UNIQUE_RIDES),/*Unique*/
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_transport_ride_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, 300, 213),
    MakeWidget({30,  195}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({200,  195}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({138,  185}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL ), /*Delete*/
    MakeWidget({ 195,  67}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MIN_RIDE_NUM), /*ride min num*/
    MakeWidget({ 195,  87}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MAX_RIDE_NUM), /*ride max num*/
    MakeWidget({282,  147}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_UNIQUE_RIDES),/*Unique*/
    MakeWidget({282,  167}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_FINISH_RIDES),/*Finish Existing Rides*/
    MakeWidget({ 195,  107}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MIN_TRACK_LENGTH), /*ride min length*/
    MakeWidget({ 195,  127}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MAX_TRACK_LENGTH), /*ride max length*/
    WIDGETS_END,
};

static rct_widget window_editor_objective_options_water_ride_goal_widets[] = {
    WINDOW_SHIM(STR_ADD_OBJECTIVE, 300, 273),
    MakeWidget({30,  255}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_GOAL_GROUP_CONFIRM ),/*Confirm*/
    MakeWidget({200,  255}, {70,  12}, WindowWidgetType::Button,   WindowColour::Secondary,STR_SAVE_PROMPT_CANCEL ), /*Cancel*/
    MakeWidget({138,  245}, {24,  24}, WindowWidgetType::ImgBtn,   WindowColour::Secondary,SPR_DEMOLISH, STR_REMOVE_GOAL ), /*Delete*/
    MakeWidget({ 195,  87}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MIN_RIDE_NUM), /*ride min num*/
    MakeWidget({ 195,  107}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MAX_RIDE_NUM), /*ride max num*/
    MakeWidget({282,  207}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_UNIQUE_RIDES),/*Unique*/
    MakeWidget({282,  227}, {12,  12}, WindowWidgetType::Checkbox,   WindowColour::Secondary,STR_NONE, STR_FINISH_RIDES),/*Finish Existing Rides*/
    MakeWidget({ 195,  127}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MIN_TRACK_LENGTH), /*ride min length*/
    MakeWidget({ 195,  147}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MAX_TRACK_LENGTH), /*ride max length*/
    MakeWidget({ 195,  167}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MIN_EXCITEMENT_RATING), /*ride min excitement*/
    MakeWidget({ 195,  187}, {100,  12}, WindowWidgetType::TextBox,   WindowColour::Secondary, STR_NONE, STR_MAX_EXCITEMENT_RATING), /*ride max excitement*/
    WIDGETS_END,
};

#pragma endregion

#pragma region Events

static void WindowEditorObjectiveOptionsAddObjectiveMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowEditorObjectiveOptionsAddObjectiveResize(rct_window *w);
static void WindowEditorObjectiveOptionsAddObjectiveMousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowEditorObjectiveOptionsAddObjectiveDropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void WindowEditorObjectiveOptionsAddObjectiveInvalidate(rct_window *w);
static void WindowEditorObjectiveOptionsAddObjectivePaint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowEditorObjectiveOptionsAddObjectiveTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text);
static void WindowEditorObjectiveOptionsAddObjectiveScrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex);
static void WindowEditorObjectiveOptionsAddObjectiveScrollgetheight(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void WindowEditorObjectiveOptionsAddObjectivecrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowEditorObjectiveOptionsAddObjectiveScrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);


static rct_window_event_list window_objective_options_add_objectives_events([](auto& events)
{
    events.mouse_up = &WindowEditorObjectiveOptionsAddObjectiveMouseup;
    events.resize = &WindowEditorObjectiveOptionsAddObjectiveResize;
    events.mouse_down = &WindowEditorObjectiveOptionsAddObjectiveMousedown;
    events.dropdown = &WindowEditorObjectiveOptionsAddObjectiveDropdown;
    events.invalidate = &WindowEditorObjectiveOptionsAddObjectiveInvalidate;
    events.paint = &WindowEditorObjectiveOptionsAddObjectivePaint;
    events.text_input = &WindowEditorObjectiveOptionsAddObjectiveTextinput;    
    events.scroll_paint = &WindowEditorObjectiveOptionsAddObjectiveScrollpaint;
    events.get_scroll_size = &WindowEditorObjectiveOptionsAddObjectiveScrollgetheight;
    events.scroll_mousedown = &WindowEditorObjectiveOptionsAddObjectivecrollmousedown;
    events.scroll_mouseover = &WindowEditorObjectiveOptionsAddObjectiveScrollmouseover;
});


#pragma endregion

#pragma region Enabled widgets

static uint64_t window_editor_objective_options_add_objective_disabled_widgets[] = {
    //0 for gentle rides
    (1ULL << WIDX_RIDE_EXCITEMENT_MIN) | 
    (1ULL << WIDX_RIDE_EXCITEMENT_MAX) |
    (1ULL << WIDX_RIDE_INTENSITY_MIN) | 
    (1ULL << WIDX_RIDE_INTENSITY_MAX) |
    (1ULL << WIDX_RIDE_NAUSEA_MIN) | 
    (1ULL << WIDX_RIDE_NAUSEA_MAX) |    
    (1ULL << WIDX_RIDE_LENGTH_MIN) | 
    (1ULL << WIDX_RIDE_LENGTH_MAX),

    //1 for research ride
    (1ULL << WIDX_BUILD_RIDE_UNIQUE) |
    (1ULL << WIDX_BUILD_RIDE_BEST),

    //2 for thrill rides
    (1ULL << WIDX_RIDE_EXCITEMENT_MIN) | 
    (1ULL << WIDX_RIDE_EXCITEMENT_MAX) |
    (1ULL << WIDX_RIDE_INTENSITY_MIN) | 
    (1ULL << WIDX_RIDE_INTENSITY_MAX) |
    (1ULL << WIDX_RIDE_NAUSEA_MIN) | 
    (1ULL << WIDX_RIDE_NAUSEA_MAX) |    
    (1ULL << WIDX_RIDE_LENGTH_MIN) | 
    (1ULL << WIDX_RIDE_LENGTH_MAX) |
    (1ULL << WIDX_RIDE_FINISHED_RIDES),

    //3 for transport rides
    (1ULL << WIDX_RIDE_EXCITEMENT_MIN) | 
    (1ULL << WIDX_RIDE_EXCITEMENT_MAX) |
    (1ULL << WIDX_RIDE_INTENSITY_MIN) | 
    (1ULL << WIDX_RIDE_INTENSITY_MAX) |
    (1ULL << WIDX_RIDE_NAUSEA_MIN) |
    (1ULL << WIDX_RIDE_NAUSEA_MAX),

    //4 for water rides
    (1ULL << WIDX_RIDE_INTENSITY_MIN) | 
    (1ULL << WIDX_RIDE_INTENSITY_MAX) |
    (1ULL << WIDX_RIDE_NAUSEA_MIN) |
    (1ULL << WIDX_RIDE_NAUSEA_MAX),  

};



static uint64_t window_editor_objective_options_page_add_objective_hold_down_widgets[] = {
    (1ULL << WIDX_VALUE_WARNING_WEEKS_INCREASE) | //0 for income goals, park value, rating and guestnum
    (1ULL << WIDX_VALUE_WARNING_WEEKS_DECREASE),

    (1ULL << WIDX_NUM_AWARDS_INCREASE) | //1 for num awards
    (1ULL << WIDX_NUM_AWARDS_DECREASE),

};
// clang-format on

#pragma endregion

rct_window* WindowEditorObjectiveOptionsAddObjectiveOpen(ObjectiveGoalGroup* _group, GoalID _goalID)
{
    editMode = false;
    rct_window* w;

    w = window_bring_to_front_by_class(WC_EDITOR_OBJECTIVE_OPTIONS_ADD_OBJECTIVE);
    if (w != nullptr)
        return w;

    w = WindowCreateCentred(
        250, 302, &window_objective_options_add_objectives_events, WC_EDITOR_OBJECTIVE_OPTIONS_ADD_OBJECTIVE,
        WF_STICK_TO_FRONT);

    group = _group;
    goalID = _goalID;
    switch (_goalID)
    {
        case GoalID::AwardGoal:
            goal = std::make_shared<ObjectiveAwardGoal>(AwardType::BestValue, true);
            w->widgets = window_editor_objective_options_award_goal_widets;
            window_editor_objective_options_award_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
            break;
        case GoalID::CoasterGoal:
            goal = std::make_shared<ObjectiveCoasterGoal>(3, 0, 100, 0, 6, 0, 0, 0, 0, 0, true, false);
            w->widgets = window_editor_objective_options_coaster_goal_widets;
            window_editor_objective_options_coaster_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_NUM_MIN].string = textbox1Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_NUM_MAX].string = textbox2Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_LENGTH_MIN].string = textbox3Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_LENGTH_MAX].string = textbox4Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_EXCITEMENT_MIN].string = textbox5Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_EXCITEMENT_MAX].string = textbox6Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_INTENSITY_MIN].string = textbox7Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_INTENSITY_MAX].string = textbox8Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_NAUSEA_MIN].string = textbox9Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_NAUSEA_MAX].string = textbox10Buffer;
            strcpy(
                textbox1Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMinNumRidesGoal()).c_str());
            strcpy(
                textbox2Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMaxNumRidesGoal()).c_str());

            if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal()))
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal()))
                        .c_str());
            }
            else
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal())
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal())
                        .c_str());
            }
            strcpy(
                textbox5Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->GetMinRideExcitementGoal()).c_str());
            strcpy(
                textbox6Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->GetMaxRideExcitementGoal()).c_str());
            strcpy(
                textbox7Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMinRideIntensityGoal()).c_str());
            strcpy(
                textbox8Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMaxRideIntensityGoal()).c_str());
            strcpy(
                textbox9Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMinRideNauseaGoal()).c_str());
            strcpy(
                textbox10Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMaxRideNauseaGoal()).c_str());
            break;
        case GoalID::CompleteResearchGoal:
            goal = std::make_shared<ObjectiveCompleteResearchGoal>();
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
            break;
        case GoalID::FoodProfitGoal:
            goal = std::make_shared<ObjectiveFoodProfitGoal>(100, Sign::BiggerThan, GoalType::Goal, 4);
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::FunGoal:
            goal = std::make_shared<ObjectiveFunGoal>();
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
            break;
        case GoalID::GentleRidesGoal:
            goal = std::make_shared<ObjectiveGentleRidesGoal>(3, 0, true, false);
            w->widgets = window_editor_objective_options_gentle_rides_goal_widets;
            window_editor_objective_options_gentle_rides_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->disabled_widgets = window_editor_objective_options_add_objective_disabled_widgets[0];
            w->hold_down_widgets = 0;
            window_editor_objective_options_gentle_rides_goal_widets[WIDX_RIDE_NUM_MIN].string = textbox1Buffer;
            window_editor_objective_options_gentle_rides_goal_widets[WIDX_RIDE_NUM_MAX].string = textbox2Buffer;
            strcpy(
                textbox1Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMinNumRidesGoal()).c_str());
            strcpy(
                textbox2Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMaxNumRidesGoal()).c_str());
            break;
        case GoalID::GuestNumGoal:
            goal = std::make_shared<ObjectiveGuestNumGoal>(250, Sign::BiggerThan, GoalType::Goal, 4);
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::ProfitGoal:
            goal = std::make_shared<ObjectiveProfitGoal>(2500, Sign::BiggerThan, GoalType::Goal, 4);
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::MerchandiseProfitGoal:
            goal = std::make_shared<ObjectiveMerchandiseProfitGoal>(100, Sign::BiggerThan, GoalType::Goal, 4);
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::NoDeathsGoal:
            goal = std::make_shared<ObjectiveNoDeathsGoal>();
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
            break;
        case GoalID::NoDebtGoal:
            goal = std::make_shared<ObjectiveNoDebtGoal>();
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
            break;
        case GoalID::NoExtremeRides:
            goal = std::make_shared<ObjectiveNoExtremeRidesGoal>();
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
            break;
        case GoalID::NoDuplicateRidesGoal:
            goal = std::make_shared<ObjectiveNoDuplicateRidesGoal>();
            w->widgets = window_editor_objective_options_no_duplicate_rides_goal_widets;
            window_editor_objective_options_no_duplicate_rides_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
            break;
        case GoalID::NoNegativeAwardsGoal:
            goal = std::make_shared<ObjectiveNoNegativeAwardsGoal>();
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
            break;
        case GoalID::NumPositiveAwardsGoal:
            goal = std::make_shared<ObjectiveNumPositiveAwardsGoal>(3);
            w->widgets = window_editor_objective_options_num_awards_goal_widets;
            window_editor_objective_options_num_awards_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[1];
            break;
        case GoalID::ParkEntryProfitGoal:
            goal = std::make_shared<ObjectiveParkEntryProfitGoal>(2000, Sign::BiggerThan, GoalType::Goal, 4);
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::ParkRatingGoal:
            goal = std::make_shared<ObjectiveParkRatingGoal>(600, Sign::BiggerThan, GoalType::Goal, 4);
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::ParkSizeGoal:
            goal = std::make_shared<ObjectiveParkSizeGoal>(1, Sign::BiggerThan);
            w->widgets = window_editor_objective_options_park_size_goal_widets;
            window_editor_objective_options_park_size_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
            break;
        case GoalID::ParkValueGoal:
            goal = std::make_shared<ObjectiveParkValueGoal>(1000000, Sign::BiggerThan, GoalType::Goal, 4);
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::RepayLoanGoal:
            goal = std::make_shared<ObjectiveRepayLoanGoal>();
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
            break;
        case GoalID::ResearchCertainRide:
        {
            int i = 0;
            if (gResearchItemsUninvented.size() == 0)
            {
                window_close(w);
                context_show_error(STR_ERROR_NO_RIDES_FOR_RESEARCH, STR_NONE, {});
                return nullptr;
            }
            while (gResearchItemsUninvented[i].type != Research::EntryType::Ride)
            {
                i++;
                if (i == gResearchItemsUninvented.size())
                {
                    window_close(w);
                    context_show_error(STR_ERROR_NO_RIDES_FOR_RESEARCH, STR_NONE, {});
                    return nullptr;
                }
            }
            goal = std::make_shared<ObjectiveResearchRide>(
                gResearchItemsUninvented[i].baseRideType, gResearchItemsUninvented[i].entryIndex);
            w->widgets = window_editor_objective_options_research_ride_goal_widets;
            window_editor_objective_options_research_ride_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->disabled_widgets = window_editor_objective_options_add_objective_disabled_widgets[1];
            w->hold_down_widgets = 0;
        }
        break;
        case GoalID::RideTicketProfitGoal:
            goal = std::make_shared<ObjectiveRideTicketProfitGoal>(2000, Sign::BiggerThan, GoalType::Goal, 4);
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::SpecificRideGoal:
        {
            bool found = false;
            uint8_t baseRideType;
            ObjectEntryIndex entryIndex;
            for (auto item : gResearchItemsInvented)
            {
                if (item.type == Research::EntryType::Ride)
                {
                    found = true;
                    baseRideType = item.baseRideType;
                    entryIndex = item.entryIndex;
                    break;
                }
            }
            if (!found)
            {
                for (auto item : gResearchItemsUninvented)
                {
                    if (item.type == Research::EntryType::Ride)
                    {
                        found = true;
                        baseRideType = item.baseRideType;
                        entryIndex = item.entryIndex;
                        break;
                    }
                }
            }
            if (!found)
            {
                window_close(w);
                context_show_error(STR_ERROR_NO_RIDES_FOR_TO_BUILD, STR_NONE, {});
                return nullptr;
            }
            goal = std::make_shared<ObjectiveSpecificRideGoal>(baseRideType, entryIndex, false, false);
            w->widgets = window_editor_objective_options_specific_ride_goal_widets;
            window_editor_objective_options_specific_ride_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
        }
        break;
        case GoalID::SpecificTrackedRideGoal:
        {
            bool found = false;
            uint8_t baseRideType = 0;
            ObjectEntryIndex entryIndex;
            for (auto item : gResearchItemsInvented)
            {
                if (item.type == Research::EntryType::Ride
                    && !GetRideTypeDescriptor(item.baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
                {
                    found = true;
                    baseRideType = item.baseRideType;
                    entryIndex = item.entryIndex;
                    break;
                }
            }
            if (!found)
            {
                for (auto item : gResearchItemsUninvented)
                {
                    if (item.type == Research::EntryType::Ride
                        && !GetRideTypeDescriptor(item.baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
                    {
                        found = true;
                        baseRideType = item.baseRideType;
                        entryIndex = item.entryIndex;
                        break;
                    }
                }
            }
            if (!found)
            {
                window_close(w);
                context_show_error(STR_ERROR_NO_RIDES_FOR_TO_BUILD, STR_NONE, {});
                return nullptr;
            }
            if (GetRideTypeDescriptor(baseRideType).HasFlag(RIDE_TYPE_FLAG_HAS_DROPS))
                goal = std::make_shared<ObjectiveSpecificTrackedRideGoal>(
                    baseRideType, entryIndex, true, 1, 0, 15, 0, 300, 0, 6, 0, 0, 10, 0, 0, 20, 0, 1, 0);
            else
                goal = std::make_shared<ObjectiveSpecificTrackedRideGoal>(
                    baseRideType, entryIndex, true, 0, 0, 0, 0, 300, 0, 3, 0, 0, 10, 0, 0, 0, 0, 0, 0);
            w->widgets = window_editor_objective_options_tracked_ride_goal_widets;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = 0;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_DROPS_MIN].string = textbox1Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_DROPS_MAX].string = textbox2Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_HEIGHT_MIN].string = textbox3Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_HEIGHT_MAX].string = textbox4Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_LENGTH_MIN].string = textbox5Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_LENGTH_MAX].string = textbox6Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_EXCITEMENT_MIN].string = textbox7Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_EXCITEMENT_MAX].string = textbox8Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_INTENSITY_MIN].string = textbox9Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_INTENSITY_MAX].string = textbox10Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_NAUSEA_MIN].string = textbox11Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_NAUSEA_MAX].string = textbox12Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_SPEED_MIN].string = textbox13Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_SPEED_MAX].string = textbox14Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_INVERSIONS_MIN].string = textbox15Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_INVERSIONS_MAX].string = textbox16Buffer;
            strcpy(
                textbox1Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinDropCountGoal())
                    .c_str());
            strcpy(
                textbox2Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxDropCountGoal())
                    .c_str());
            if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(
                        metres_to_feet(
                            std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinDropHeightGoal()))
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(
                        metres_to_feet(
                            std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxDropHeightGoal()))
                        .c_str());
                strcpy(
                    textbox5Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinLengthGoal()))
                        .c_str());
                strcpy(
                    textbox6Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxLengthGoal()))
                        .c_str());
                strcpy(
                    textbox13Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinSpeedGoal())
                        .c_str());
                strcpy(
                    textbox14Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxSpeedGoal())
                        .c_str());
            }
            else
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinDropHeightGoal())
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxDropHeightGoal())
                        .c_str());
                strcpy(
                    textbox5Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinLengthGoal())
                        .c_str());
                strcpy(
                    textbox6Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxLengthGoal())
                        .c_str());
                strcpy(
                    textbox13Buffer,
                    std::to_string(
                        mph_to_kmph(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinSpeedGoal()))
                        .c_str());
                strcpy(
                    textbox14Buffer,
                    std::to_string(
                        mph_to_kmph(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxSpeedGoal()))
                        .c_str());
            }
            strcpy(
                textbox7Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinExcitementGoal())
                    .c_str());
            strcpy(
                textbox8Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxExcitementGoal())
                    .c_str());
            strcpy(
                textbox9Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinIntensityGoal())
                    .c_str());
            strcpy(
                textbox10Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxIntensityGoal())
                    .c_str());
            strcpy(
                textbox11Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinNauseaGoal()).c_str());
            strcpy(
                textbox12Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxNauseaGoal()).c_str());
            strcpy(
                textbox15Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinNumInversions())
                    .c_str());
            strcpy(
                textbox16Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxNumInversions())
                    .c_str());
        }
        break;
        case GoalID::StallProfitGoal:
            goal = std::make_shared<ObjectiveStallProfitGoal>(500, Sign::BiggerThan, GoalType::Goal, 4);
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::ThrillRidesGoal:
            goal = std::make_shared<ObjectiveThrillRidesGoal>(3, 0, true);
            w->widgets = window_editor_objective_options_thrill_ride_goal_widets;
            window_editor_objective_options_thrill_ride_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->disabled_widgets = window_editor_objective_options_add_objective_disabled_widgets[2];
            w->hold_down_widgets = 0;
            window_editor_objective_options_thrill_ride_goal_widets[WIDX_RIDE_NUM_MIN].string = textbox1Buffer;
            window_editor_objective_options_thrill_ride_goal_widets[WIDX_RIDE_NUM_MAX].string = textbox2Buffer;
            strcpy(
                textbox1Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMinNumRidesGoal()).c_str());
            strcpy(
                textbox2Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMaxNumRidesGoal()).c_str());
            break;
        case GoalID::TransportRidesGoal:
            goal = std::make_shared<ObjectiveTransportRidesGoal>(3, 0, 500, 0, true, false);
            w->widgets = window_editor_objective_options_transport_ride_goal_widets;
            window_editor_objective_options_transport_ride_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->disabled_widgets = window_editor_objective_options_add_objective_disabled_widgets[3];
            window_editor_objective_options_transport_ride_goal_widets[WIDX_RIDE_NUM_MIN].string = textbox1Buffer;
            window_editor_objective_options_transport_ride_goal_widets[WIDX_RIDE_NUM_MAX].string = textbox2Buffer;
            window_editor_objective_options_transport_ride_goal_widets[WIDX_RIDE_LENGTH_MIN].string = textbox3Buffer;
            window_editor_objective_options_transport_ride_goal_widets[WIDX_RIDE_LENGTH_MAX].string = textbox4Buffer;
            strcpy(
                textbox1Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMinNumRidesGoal()).c_str());
            strcpy(
                textbox2Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMaxNumRidesGoal()).c_str());

            if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal()))
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal()))
                        .c_str());
            }
            else
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal())
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal())
                        .c_str());
            }
            w->hold_down_widgets = 0;
            break;
        case GoalID::WaterRidesGoal:
            goal = std::make_shared<ObjectiveWaterRidesGoal>(3, 0, 100, 0, 4, 0, true, false);
            w->widgets = window_editor_objective_options_water_ride_goal_widets;
            window_editor_objective_options_water_ride_goal_widets[WIDX_DELETE].type = WindowWidgetType::Empty;
            w->disabled_widgets = window_editor_objective_options_add_objective_disabled_widgets[4];
            w->hold_down_widgets = 0;
            window_editor_objective_options_water_ride_goal_widets[WIDX_RIDE_NUM_MIN].string = textbox1Buffer;
            window_editor_objective_options_water_ride_goal_widets[WIDX_RIDE_NUM_MAX].string = textbox2Buffer;
            window_editor_objective_options_water_ride_goal_widets[WIDX_RIDE_LENGTH_MIN].string = textbox3Buffer;
            window_editor_objective_options_water_ride_goal_widets[WIDX_RIDE_LENGTH_MAX].string = textbox4Buffer;
            window_editor_objective_options_water_ride_goal_widets[WIDX_RIDE_EXCITEMENT_MIN].string = textbox5Buffer;
            window_editor_objective_options_water_ride_goal_widets[WIDX_RIDE_EXCITEMENT_MAX].string = textbox6Buffer;
            strcpy(
                textbox1Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMinNumRidesGoal()).c_str());
            strcpy(
                textbox2Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMaxNumRidesGoal()).c_str());

            if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal()))
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal()))
                        .c_str());
            }
            else
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal())
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal())
                        .c_str());
            }
            strcpy(
                textbox5Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->GetMinRideExcitementGoal()).c_str());
            strcpy(
                textbox6Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->GetMaxRideExcitementGoal()).c_str());
            break;
        default:
            break;
    }
    w->disabled_widgets &= (1ULL << WIDX_DELETE);
    // w->enabled_widgets &= ~(1ULL << WIDX_DELETE);

    w->widgets[WIDX_ADD_OBJECTIVE_TITLE].text = STR_ADD_OBJECTIVE;
    w->pressed_widgets = 0;
    w->no_list_items = 0;
    w->number = 0;
    w->page = 0;
    w->frame_no = 0;
    w->list_information_type = 0;
    w->picked_peep_frame = 0;
    w->highlighted_item = 0;
    w->selected_list_item = -1;

    w->Invalidate();
    window_event_resize_call(w);
    window_event_invalidate_call(w);
    WindowInitScrollWidgets(w);

    return w;
}

rct_window* WindowEditorObjectiveOptionsEditObjectiveOpen(
    ObjectiveGoalGroup* _group, ObjectiveGoalPtr _goal, uint32_t _goalIndex)
{
    editMode = true;
    rct_window* w;

    w = window_bring_to_front_by_class(WC_EDITOR_OBJECTIVE_OPTIONS_ADD_OBJECTIVE);
    if (w != nullptr)
        return w;
    originalGoalIndex = _goalIndex;

    group = _group;
    w = WindowCreateCentred(
        250, 302, &window_objective_options_add_objectives_events, WC_EDITOR_OBJECTIVE_OPTIONS_ADD_OBJECTIVE,
        WF_STICK_TO_FRONT);

    goalID = _goal->GetGoalID();
    switch (goalID)
    {
        case GoalID::AwardGoal:
            goal = std::make_shared<ObjectiveAwardGoal>(*(std::static_pointer_cast<ObjectiveAwardGoal>(_goal)));
            w->widgets = window_editor_objective_options_award_goal_widets;
            window_editor_objective_options_award_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            break;
        case GoalID::CoasterGoal:
            goal = std::make_shared<ObjectiveCoasterGoal>(*(std::static_pointer_cast<ObjectiveCoasterGoal>(_goal)));
            w->widgets = window_editor_objective_options_coaster_goal_widets;
            window_editor_objective_options_coaster_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_NUM_MIN].string = textbox1Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_NUM_MAX].string = textbox2Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_LENGTH_MIN].string = textbox3Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_LENGTH_MAX].string = textbox4Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_EXCITEMENT_MIN].string = textbox5Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_EXCITEMENT_MAX].string = textbox6Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_INTENSITY_MIN].string = textbox7Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_INTENSITY_MAX].string = textbox8Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_NAUSEA_MIN].string = textbox9Buffer;
            window_editor_objective_options_coaster_goal_widets[WIDX_RIDE_NAUSEA_MAX].string = textbox10Buffer;
            strcpy(
                textbox1Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMinNumRidesGoal()).c_str());
            strcpy(
                textbox2Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMaxNumRidesGoal()).c_str());

            if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal()))
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal()))
                        .c_str());
            }
            else
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal())
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal())
                        .c_str());
            }
            strcpy(
                textbox5Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->GetMinRideExcitementGoal()).c_str());
            strcpy(
                textbox6Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->GetMaxRideExcitementGoal()).c_str());
            strcpy(
                textbox7Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMinRideIntensityGoal()).c_str());
            strcpy(
                textbox8Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMaxRideIntensityGoal()).c_str());
            strcpy(
                textbox9Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMinRideNauseaGoal()).c_str());
            strcpy(
                textbox10Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMaxRideNauseaGoal()).c_str());
            break;
        case GoalID::CompleteResearchGoal:
            goal = std::make_shared<ObjectiveCompleteResearchGoal>(
                *(std::static_pointer_cast<ObjectiveCompleteResearchGoal>(_goal)));
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            break;
        case GoalID::FoodProfitGoal:
            goal = std::make_shared<ObjectiveFoodProfitGoal>(*(std::static_pointer_cast<ObjectiveFoodProfitGoal>(_goal)));
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::FunGoal:
            goal = std::make_shared<ObjectiveFunGoal>(*(std::static_pointer_cast<ObjectiveFunGoal>(_goal)));
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            break;
        case GoalID::GentleRidesGoal:
            goal = std::make_shared<ObjectiveGentleRidesGoal>(*(std::static_pointer_cast<ObjectiveGentleRidesGoal>(_goal)));
            w->widgets = window_editor_objective_options_gentle_rides_goal_widets;
            window_editor_objective_options_gentle_rides_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->disabled_widgets = window_editor_objective_options_add_objective_disabled_widgets[0];
            w->hold_down_widgets = 0;
            window_editor_objective_options_gentle_rides_goal_widets[WIDX_RIDE_NUM_MIN].string = textbox1Buffer;
            window_editor_objective_options_gentle_rides_goal_widets[WIDX_RIDE_NUM_MAX].string = textbox2Buffer;
            strcpy(
                textbox1Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMinNumRidesGoal()).c_str());
            strcpy(
                textbox2Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMaxNumRidesGoal()).c_str());
            break;
        case GoalID::GuestNumGoal:
            goal = std::make_shared<ObjectiveGuestNumGoal>(*(std::static_pointer_cast<ObjectiveGuestNumGoal>(_goal)));
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::ProfitGoal:
            goal = std::make_shared<ObjectiveProfitGoal>(*(std::static_pointer_cast<ObjectiveProfitGoal>(_goal)));
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::MerchandiseProfitGoal:
            goal = std::make_shared<ObjectiveMerchandiseProfitGoal>(
                *(std::static_pointer_cast<ObjectiveMerchandiseProfitGoal>(_goal)));
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::NoDeathsGoal:
            goal = std::make_shared<ObjectiveNoDeathsGoal>(*(std::static_pointer_cast<ObjectiveNoDeathsGoal>(_goal)));
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            break;
        case GoalID::NoDebtGoal:
            goal = std::make_shared<ObjectiveNoDebtGoal>(*(std::static_pointer_cast<ObjectiveNoDebtGoal>(_goal)));
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            break;
        case GoalID::NoDuplicateRidesGoal:
            goal = std::make_shared<ObjectiveNoDuplicateRidesGoal>(
                *(std::static_pointer_cast<ObjectiveNoDuplicateRidesGoal>(_goal)));
            w->widgets = window_editor_objective_options_no_duplicate_rides_goal_widets;
            window_editor_objective_options_no_duplicate_rides_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            break;
        case GoalID::NoExtremeRides:
            goal = std::make_shared<ObjectiveNoExtremeRidesGoal>(
                *(std::static_pointer_cast<ObjectiveNoExtremeRidesGoal>(_goal)));
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            break;
        case GoalID::NoNegativeAwardsGoal:
            goal = std::make_shared<ObjectiveNoNegativeAwardsGoal>(
                *(std::static_pointer_cast<ObjectiveNoNegativeAwardsGoal>(_goal)));
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            break;
        case GoalID::NumPositiveAwardsGoal:
            goal = std::make_shared<ObjectiveNumPositiveAwardsGoal>(
                *(std::static_pointer_cast<ObjectiveNumPositiveAwardsGoal>(_goal)));
            w->widgets = window_editor_objective_options_num_awards_goal_widets;
            window_editor_objective_options_num_awards_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[1];
            break;
        case GoalID::ParkEntryProfitGoal:
            goal = std::make_shared<ObjectiveParkEntryProfitGoal>(
                *(std::static_pointer_cast<ObjectiveParkEntryProfitGoal>(_goal)));
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::ParkRatingGoal:
            goal = std::make_shared<ObjectiveParkRatingGoal>(*(std::static_pointer_cast<ObjectiveParkRatingGoal>(_goal)));
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::ParkSizeGoal:
            goal = std::make_shared<ObjectiveParkSizeGoal>(*(std::static_pointer_cast<ObjectiveParkSizeGoal>(_goal)));
            w->widgets = window_editor_objective_options_park_size_goal_widets;
            window_editor_objective_options_park_size_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            break;
        case GoalID::ParkValueGoal:
            goal = std::make_shared<ObjectiveParkValueGoal>(*(std::static_pointer_cast<ObjectiveParkValueGoal>(_goal)));
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::RepayLoanGoal:
            goal = std::make_shared<ObjectiveRepayLoanGoal>(*(std::static_pointer_cast<ObjectiveRepayLoanGoal>(_goal)));
            w->widgets = window_editor_objective_options_simple_goal_widets;
            window_editor_objective_options_simple_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            break;
        case GoalID::ResearchCertainRide:
            goal = std::make_shared<ObjectiveResearchRide>(*(std::static_pointer_cast<ObjectiveResearchRide>(_goal)));
            w->widgets = window_editor_objective_options_research_ride_goal_widets;
            window_editor_objective_options_research_ride_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->disabled_widgets = window_editor_objective_options_add_objective_disabled_widgets[1];
            w->hold_down_widgets = 0;
            break;
        case GoalID::RideTicketProfitGoal:
            goal = std::make_shared<ObjectiveRideTicketProfitGoal>(
                *(std::static_pointer_cast<ObjectiveRideTicketProfitGoal>(_goal)));
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::SpecificRideGoal:
            goal = std::make_shared<ObjectiveSpecificRideGoal>(*(std::static_pointer_cast<ObjectiveSpecificRideGoal>(_goal)));
            w->widgets = window_editor_objective_options_specific_ride_goal_widets;
            window_editor_objective_options_specific_ride_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            break;
        case GoalID::SpecificTrackedRideGoal:
            goal = std::make_shared<ObjectiveSpecificTrackedRideGoal>(
                *(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(_goal)));
            w->widgets = window_editor_objective_options_tracked_ride_goal_widets;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = 0;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_DROPS_MIN].string = textbox1Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_DROPS_MAX].string = textbox2Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_HEIGHT_MIN].string = textbox3Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_HEIGHT_MAX].string = textbox4Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_LENGTH_MIN].string = textbox5Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_LENGTH_MAX].string = textbox6Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_EXCITEMENT_MIN].string = textbox7Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_EXCITEMENT_MAX].string = textbox8Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_INTENSITY_MIN].string = textbox9Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_INTENSITY_MAX].string = textbox10Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_NAUSEA_MIN].string = textbox11Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_NAUSEA_MAX].string = textbox12Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_SPEED_MIN].string = textbox13Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_SPEED_MAX].string = textbox14Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_INVERSIONS_MIN].string = textbox15Buffer;
            window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_INVERSIONS_MAX].string = textbox16Buffer;
            strcpy(
                textbox1Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinDropCountGoal())
                    .c_str());
            strcpy(
                textbox2Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxDropCountGoal())
                    .c_str());
            if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(
                        metres_to_feet(
                            std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinDropHeightGoal()))
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(
                        metres_to_feet(
                            std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxDropHeightGoal()))
                        .c_str());
                strcpy(
                    textbox5Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinLengthGoal()))
                        .c_str());
                strcpy(
                    textbox6Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxLengthGoal()))
                        .c_str());
                strcpy(
                    textbox13Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinSpeedGoal())
                        .c_str());
                strcpy(
                    textbox14Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxSpeedGoal())
                        .c_str());
            }
            else
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinDropHeightGoal())
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxDropHeightGoal())
                        .c_str());
                strcpy(
                    textbox5Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinLengthGoal())
                        .c_str());
                strcpy(
                    textbox6Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxLengthGoal())
                        .c_str());
                strcpy(
                    textbox13Buffer,
                    std::to_string(
                        mph_to_kmph(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinSpeedGoal()))
                        .c_str());
                strcpy(
                    textbox14Buffer,
                    std::to_string(
                        mph_to_kmph(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxSpeedGoal()))
                        .c_str());
            }
            strcpy(
                textbox7Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinExcitementGoal())
                    .c_str());
            strcpy(
                textbox8Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxExcitementGoal())
                    .c_str());
            strcpy(
                textbox9Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinIntensityGoal())
                    .c_str());
            strcpy(
                textbox10Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxIntensityGoal())
                    .c_str());
            strcpy(
                textbox11Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinNauseaGoal()).c_str());
            strcpy(
                textbox12Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxNauseaGoal()).c_str());
            strcpy(
                textbox15Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinNumInversions())
                    .c_str());
            strcpy(
                textbox16Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxNumInversions())
                    .c_str());
            break;
        case GoalID::StallProfitGoal:
            goal = std::make_shared<ObjectiveStallProfitGoal>(*(std::static_pointer_cast<ObjectiveStallProfitGoal>(_goal)));
            w->widgets = window_editor_objective_options_value_sign_goal_widets;
            window_editor_objective_options_value_sign_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->hold_down_widgets = window_editor_objective_options_page_add_objective_hold_down_widgets[0];
            break;
        case GoalID::ThrillRidesGoal:
            goal = std::make_shared<ObjectiveThrillRidesGoal>(*(std::static_pointer_cast<ObjectiveThrillRidesGoal>(_goal)));
            w->widgets = window_editor_objective_options_thrill_ride_goal_widets;
            window_editor_objective_options_thrill_ride_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->disabled_widgets = window_editor_objective_options_add_objective_disabled_widgets[2];
            w->hold_down_widgets = 0;
            window_editor_objective_options_thrill_ride_goal_widets[WIDX_RIDE_NUM_MIN].string = textbox1Buffer;
            window_editor_objective_options_thrill_ride_goal_widets[WIDX_RIDE_NUM_MAX].string = textbox2Buffer;
            strcpy(
                textbox1Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMinNumRidesGoal()).c_str());
            strcpy(
                textbox2Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMaxNumRidesGoal()).c_str());
            break;
        case GoalID::TransportRidesGoal:
            goal = std::make_shared<ObjectiveTransportRidesGoal>(
                *(std::static_pointer_cast<ObjectiveTransportRidesGoal>(_goal)));
            w->widgets = window_editor_objective_options_transport_ride_goal_widets;
            window_editor_objective_options_transport_ride_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->disabled_widgets = window_editor_objective_options_add_objective_disabled_widgets[3];
            w->hold_down_widgets = 0;
            window_editor_objective_options_transport_ride_goal_widets[WIDX_RIDE_NUM_MIN].string = textbox1Buffer;
            window_editor_objective_options_transport_ride_goal_widets[WIDX_RIDE_NUM_MAX].string = textbox2Buffer;
            window_editor_objective_options_transport_ride_goal_widets[WIDX_RIDE_LENGTH_MIN].string = textbox3Buffer;
            window_editor_objective_options_transport_ride_goal_widets[WIDX_RIDE_LENGTH_MAX].string = textbox4Buffer;
            strcpy(
                textbox1Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMinNumRidesGoal()).c_str());
            strcpy(
                textbox2Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMaxNumRidesGoal()).c_str());
            if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal()))
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal()))
                        .c_str());
            }
            else
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal())
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal())
                        .c_str());
            }
            break;
        case GoalID::WaterRidesGoal:
            goal = std::make_shared<ObjectiveWaterRidesGoal>(*(std::static_pointer_cast<ObjectiveWaterRidesGoal>(_goal)));
            w->widgets = window_editor_objective_options_water_ride_goal_widets;
            window_editor_objective_options_water_ride_goal_widets[WIDX_DELETE].type = WindowWidgetType::ImgBtn;
            w->disabled_widgets = window_editor_objective_options_add_objective_disabled_widgets[4];
            w->hold_down_widgets = 0;
            window_editor_objective_options_water_ride_goal_widets[WIDX_RIDE_NUM_MIN].string = textbox1Buffer;
            window_editor_objective_options_water_ride_goal_widets[WIDX_RIDE_NUM_MAX].string = textbox2Buffer;
            window_editor_objective_options_water_ride_goal_widets[WIDX_RIDE_LENGTH_MIN].string = textbox3Buffer;
            window_editor_objective_options_water_ride_goal_widets[WIDX_RIDE_LENGTH_MAX].string = textbox4Buffer;
            window_editor_objective_options_water_ride_goal_widets[WIDX_RIDE_EXCITEMENT_MIN].string = textbox5Buffer;
            window_editor_objective_options_water_ride_goal_widets[WIDX_RIDE_EXCITEMENT_MAX].string = textbox6Buffer;
            strcpy(
                textbox1Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMinNumRidesGoal()).c_str());
            strcpy(
                textbox2Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMaxNumRidesGoal()).c_str());
            if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal()))
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(
                        metres_to_feet(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal()))
                        .c_str());
            }
            else
            {
                strcpy(
                    textbox3Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal())
                        .c_str());
                strcpy(
                    textbox4Buffer,
                    std::to_string(std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal())
                        .c_str());
            }
            strcpy(
                textbox5Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->GetMinRideExcitementGoal()).c_str());
            strcpy(
                textbox6Buffer,
                std::to_string(std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->GetMaxRideExcitementGoal()).c_str());
            break;
        default:
            break;
    }
    w->disabled_widgets &= ~(1ULL << WIDX_DELETE); // add delete to all of them
    w->widgets[WIDX_ADD_OBJECTIVE_TITLE].text = STR_EDIT_OBJECTIVE;

    w->pressed_widgets = 0;
    w->no_list_items = 0;
    w->number = 0;
    w->page = 0;
    w->frame_no = 0;
    w->list_information_type = 0;
    w->picked_peep_frame = 0;
    w->highlighted_item = 0;
    w->selected_list_item = -1;

    w->Invalidate();
    window_event_resize_call(w);
    window_event_invalidate_call(w);
    WindowInitScrollWidgets(w);

    return w;
}

static void WindowEditorObjectiveOptionsAddObjectiveAnchorBorderWidgets(rct_window* w)
{
    w->widgets[WIDX_ADD_OBJECTIVE_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_ADD_OBJECTIVE_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_ADD_OBJECTIVE_TITLE].right = w->width - 2;
    w->widgets[WIDX_ADD_OBJECTIVE_CLOSE].left = w->width - 13;
    w->widgets[WIDX_ADD_OBJECTIVE_CLOSE].right = w->width - 3;
}

static void WindowEditorObjectiveOptionsAddObjectiveMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    rct_string_id error = STR_NONE;
    switch (widgetIndex)
    {
        case WIDX_CANCEL:
        case WIDX_ADD_OBJECTIVE_CLOSE:
            window_close(w);
            break;
        case WIDX_OKAY:
            if (editMode)
            {
                error = group->EditGoal(goal, originalGoalIndex);
                if (error == STR_NONE)
                    window_close(w);
                else
                    context_show_error(error, STR_NONE, {});
            }
            else
            {
                error = group->AddGoal(goal);
                if (error == STR_NONE)
                    window_close(w);
                else
                    context_show_error(error, STR_NONE, {});
            }
            break;
        case WIDX_DELETE:
            if (editMode)
            {
                group->DeleteGoal(originalGoalIndex);
                window_close(w);
            }
            break;
    }
    switch (goalID)
    {
        case GoalID::AwardGoal:
            if (widgetIndex == WIDX_AWARD_ANYTIME)
                std::static_pointer_cast<ObjectiveAwardGoal>(goal)->SetAtAnyTime(
                    !std::static_pointer_cast<ObjectiveAwardGoal>(goal)->GetAtAnyTime());
            break;
        case GoalID::CoasterGoal:
        case GoalID::WaterRidesGoal:
        case GoalID::TransportRidesGoal:
        case GoalID::GentleRidesGoal:
            if (widgetIndex == WIDX_RIDE_FINISHED_RIDES)
                std::static_pointer_cast<ObjectiveGentleRidesGoal>(goal)->SetFinishedExistingRidess(
                    !std::static_pointer_cast<ObjectiveGentleRidesGoal>(goal)->GetFinishedExistingRides());
            [[fallthrough]];
        case GoalID::ThrillRidesGoal:
            switch (widgetIndex)
            {
                case WIDX_RIDE_UNIQUE:
                    std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->SetMustBeUniqueTypes(
                        !std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMustBeUniqueTypes());
                    break;
                case WIDX_RIDE_NUM_MIN:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox1Buffer, 3);
                    break;
                case WIDX_RIDE_NUM_MAX:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox2Buffer, 3);
                    break;
                case WIDX_RIDE_LENGTH_MIN:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox3Buffer, 6);
                    break;
                case WIDX_RIDE_LENGTH_MAX:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox4Buffer, 6);
                    break;
                case WIDX_RIDE_EXCITEMENT_MIN:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox5Buffer, 5);
                    break;
                case WIDX_RIDE_EXCITEMENT_MAX:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox6Buffer, 5);
                    break;
                case WIDX_RIDE_INTENSITY_MIN:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox7Buffer, 5);
                    break;
                case WIDX_RIDE_INTENSITY_MAX:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox8Buffer, 5);
                    break;
                case WIDX_RIDE_NAUSEA_MIN:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox9Buffer, 5);
                    break;
                case WIDX_RIDE_NAUSEA_MAX:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox10Buffer, 5);
                    break;
            }

            break;
        case GoalID::CompleteResearchGoal:
        case GoalID::FunGoal:
        case GoalID::NoDeathsGoal:
        case GoalID::NoDebtGoal:
        case GoalID::NoDuplicateRidesGoal:
        case GoalID::NoExtremeRides:
        case GoalID::NoNegativeAwardsGoal:
        case GoalID::RepayLoanGoal:
            break; // nothing
        case GoalID::FoodProfitGoal:
        case GoalID::GuestNumGoal:
        case GoalID::ProfitGoal:
        case GoalID::MerchandiseProfitGoal:
        case GoalID::ParkEntryProfitGoal:
        case GoalID::ParkRatingGoal:
        case GoalID::ParkValueGoal:
        case GoalID::ParkSizeGoal:
        case GoalID::RideTicketProfitGoal:
        case GoalID::StallProfitGoal:
            if (widgetIndex == WIDX_VALUE_RESTRICTION && goal->GetGoalType() == GoalType::Goal)
                goal->SetGoalType(GoalType::Restriction);
            else if (widgetIndex == WIDX_VALUE_RESTRICTION && goal->GetGoalType() == GoalType::Restriction)
                goal->SetGoalType(GoalType::Goal);
            break;
        case GoalID::NumPositiveAwardsGoal:
            break;
        case GoalID::SpecificTrackedRideGoal:
            switch (widgetIndex)
            {
                case WIDX_BUILD_TRACK_DROPS_MIN:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox1Buffer, 3);
                    break;
                case WIDX_BUILD_TRACK_DROPS_MAX:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox2Buffer, 3);
                    break;
                case WIDX_BUILD_TRACK_HEIGHT_MIN:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox3Buffer, 4);
                    break;
                case WIDX_BUILD_TRACK_HEIGHT_MAX:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox4Buffer, 4);
                    break;
                case WIDX_BUILD_TRACK_LENGTH_MIN:
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                        window_start_textbox(w, widgetIndex, STR_STRING, textbox5Buffer, 7);
                    else
                        window_start_textbox(w, widgetIndex, STR_STRING, textbox5Buffer, 6);
                    break;
                case WIDX_BUILD_TRACK_LENGTH_MAX:
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                        window_start_textbox(w, widgetIndex, STR_STRING, textbox6Buffer, 7);
                    else
                        window_start_textbox(w, widgetIndex, STR_STRING, textbox6Buffer, 6);
                    break;
                case WIDX_BUILD_TRACK_EXCITEMENT_MIN:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox7Buffer, 5);
                    break;
                case WIDX_BUILD_TRACK_EXCITEMENT_MAX:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox8Buffer, 5);
                    break;
                case WIDX_BUILD_TRACK_INTENSITY_MIN:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox9Buffer, 5);
                    break;
                case WIDX_BUILD_TRACK_INTENSITY_MAX:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox10Buffer, 5);
                    break;
                case WIDX_BUILD_TRACK_NAUSEA_MIN:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox11Buffer, 5);
                    break;
                case WIDX_BUILD_TRACK_NAUSEA_MAX:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox12Buffer, 5);
                    break;
                case WIDX_BUILD_TRACK_SPEED_MIN:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox13Buffer, 4);
                    break;
                case WIDX_BUILD_TRACK_SPEED_MAX:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox14Buffer, 4);
                    break;
                case WIDX_BUILD_TRACK_INVERSIONS_MIN:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox15Buffer, 3);
                    break;
                case WIDX_BUILD_TRACK_INVERSIONS_MAX:
                    window_start_textbox(w, widgetIndex, STR_STRING, textbox16Buffer, 3);
                    break;
            }
            [[fallthrough]];
        case GoalID::SpecificRideGoal:
            if (widgetIndex == WIDX_BUILD_RIDE_UNIQUE)
                std::static_pointer_cast<ObjectiveSpecificRideGoal>(goal)->SetMustBeOnlySuchRide(
                    !std::static_pointer_cast<ObjectiveSpecificRideGoal>(goal)->GetMustBeOnlySuchRide());
            if (widgetIndex == WIDX_BUILD_RIDE_BEST && goalID == GoalID::SpecificRideGoal)
                std::static_pointer_cast<ObjectiveSpecificRideGoal>(goal)->SetBuildBestGoal(
                    !std::static_pointer_cast<ObjectiveSpecificRideGoal>(goal)->GetBuildBestGoal());
            if (widgetIndex == WIDX_BUILD_TRACK_REMOVE_PIECES && goalID == GoalID::SpecificTrackedRideGoal)
                deleteTrackPiecesMode = !deleteTrackPiecesMode;
            [[fallthrough]];
        case GoalID::ResearchCertainRide:
            break;
        default:
            break;
    }
    if (widgetIndex == WIDX_VALUE_SET_AMOUNT)
    {
        switch (goalID)
        {
            case GoalID::FoodProfitGoal:
                WindowTextInputRawOpen(
                    w, WIDX_VALUE_SET_AMOUNT, STR_ENTER_VALUE, STR_ENTER_FOOD_SALES_PROFIT, {},
                    std::to_string(std::static_pointer_cast<ObjectiveProfitGoal>(goal)->GetProfitGoal() / 10).c_str(), 7);
                break;
            case GoalID::GuestNumGoal:
                WindowTextInputRawOpen(
                    w, WIDX_VALUE_SET_AMOUNT, STR_ENTER_VALUE, STR_ENTER_GUEST_NUM, {},
                    std::to_string(std::static_pointer_cast<ObjectiveGuestNumGoal>(goal)->GetGuestNumGoal()).c_str(), 6);
                break;
            case GoalID::ProfitGoal:
                WindowTextInputRawOpen(
                    w, WIDX_VALUE_SET_AMOUNT, STR_ENTER_VALUE, STR_ENTER_TOTAL_PROFIT, {},
                    std::to_string(std::static_pointer_cast<ObjectiveProfitGoal>(goal)->GetProfitGoal() / 10).c_str(), 8);
                break;
            case GoalID::MerchandiseProfitGoal:
                WindowTextInputRawOpen(
                    w, WIDX_VALUE_SET_AMOUNT, STR_ENTER_VALUE, STR_ENTER_MERCH_PROFIT, {},
                    std::to_string(std::static_pointer_cast<ObjectiveProfitGoal>(goal)->GetProfitGoal() / 10).c_str(), 7);
                break;
            case GoalID::ParkEntryProfitGoal:
                WindowTextInputRawOpen(
                    w, WIDX_VALUE_SET_AMOUNT, STR_ENTER_VALUE, STR_ENTER_PARK_ENTRY_PROFIT, {},
                    std::to_string(std::static_pointer_cast<ObjectiveProfitGoal>(goal)->GetProfitGoal() / 10).c_str(), 7);
                break;
            case GoalID::ParkRatingGoal:
                WindowTextInputRawOpen(
                    w, WIDX_VALUE_SET_AMOUNT, STR_ENTER_VALUE, STR_ENTER_PARK_RATING, {},
                    std::to_string(std::static_pointer_cast<ObjectiveParkRatingGoal>(goal)->GetParkRatingGoal()).c_str(), 5);
                break;
            case GoalID::ParkValueGoal:
                WindowTextInputRawOpen(
                    w, WIDX_VALUE_SET_AMOUNT, STR_ENTER_VALUE, STR_ENTER_PARK_VALUE, {},
                    std::to_string(std::static_pointer_cast<ObjectiveParkValueGoal>(goal)->GetParkValueGoal() / 10).c_str(), 9);
                break;
            case GoalID::ParkSizeGoal:
                WindowTextInputRawOpen(
                    w, WIDX_VALUE_SET_AMOUNT, STR_ENTER_VALUE, STR_ENTER_PARK_SIZE, {},
                    std::to_string(10 * std::static_pointer_cast<ObjectiveParkSizeGoal>(goal)->GetParkSizeGoal()).c_str(), 8);
                break;
            case GoalID::RideTicketProfitGoal:
                WindowTextInputRawOpen(
                    w, WIDX_VALUE_SET_AMOUNT, STR_ENTER_VALUE, STR_ENTER_RIDE_TICKET_PROFIT, {},
                    std::to_string(std::static_pointer_cast<ObjectiveProfitGoal>(goal)->GetProfitGoal() / 10).c_str(), 7);
                break;
            case GoalID::StallProfitGoal:
                WindowTextInputRawOpen(
                    w, WIDX_VALUE_SET_AMOUNT, STR_ENTER_VALUE, STR_ENTER_STALL_PROFIT, {},
                    std::to_string(std::static_pointer_cast<ObjectiveProfitGoal>(goal)->GetProfitGoal() / 10).c_str(), 7);
                break;
        }
    }
}

static void WindowEditorObjectiveOptionsAddObjectiveResize(rct_window* w)
{
    switch (goalID)
    {
        case GoalID::AwardGoal:
            window_set_resize(w, 250, 132, 250, 132);
            break;
        case GoalID::CoasterGoal:
            window_set_resize(w, 300, 353, 300, 3153);
            break;
        case GoalID::NoDuplicateRidesGoal:
            window_set_resize(w, 250, 100, 250, 100);
            break;
        case GoalID::CompleteResearchGoal:
        case GoalID::FunGoal:
        case GoalID::NoDeathsGoal:
        case GoalID::NoDebtGoal:
        case GoalID::NoExtremeRides:
        case GoalID::NoNegativeAwardsGoal:
        case GoalID::RepayLoanGoal:
            window_set_resize(w, 250, 75, 250, 75);
            break;
        case GoalID::FoodProfitGoal:
        case GoalID::GuestNumGoal:
        case GoalID::ProfitGoal:
        case GoalID::MerchandiseProfitGoal:
        case GoalID::ParkEntryProfitGoal:
        case GoalID::ParkRatingGoal:
        case GoalID::ParkValueGoal:
        case GoalID::StallProfitGoal:
        case GoalID::RideTicketProfitGoal:
            window_set_resize(w, 250, 175, 250, 175);
            break;
        case GoalID::ParkSizeGoal:
            window_set_resize(w, 250, 135, 250, 135);
            break;
        case GoalID::GentleRidesGoal:
            window_set_resize(w, 300, 173, 300, 173);
            break;
        case GoalID::ResearchCertainRide:
        case GoalID::NumPositiveAwardsGoal:
            window_set_resize(w, 250, 105, 250, 105);
            break;
        case GoalID::SpecificRideGoal:
            window_set_resize(w, 250, 145, 250, 145);
            break;
        case GoalID::SpecificTrackedRideGoal:
            window_set_resize(w, 600, 412, 600, 412);
            break;
        case GoalID::ThrillRidesGoal:
            window_set_resize(w, 300, 153, 300, 153);
            break;
        case GoalID::TransportRidesGoal:
            window_set_resize(w, 300, 213, 300, 213);
            break;
        case GoalID::WaterRidesGoal:
            window_set_resize(w, 300, 273, 300, 273);
            break;
        default:
            window_set_resize(w, 250, 302, 250, 302);
            break;
    }
}

static void WindowEditorObjectiveOptionsAddObjectiveDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;
    switch (goal->GetGoalID())
    {
        case GoalID::AwardGoal:
            std::static_pointer_cast<ObjectiveAwardGoal>(goal)->SetAwardsGoal(static_cast<AwardType>(dropdownIndex));
            break;

        case GoalID::ProfitGoal:
        case GoalID::ParkEntryProfitGoal:
        case GoalID::RideTicketProfitGoal:
        case GoalID::FoodProfitGoal:
        case GoalID::MerchandiseProfitGoal:
        case GoalID::StallProfitGoal:
        case GoalID::GuestNumGoal:
        case GoalID::ParkSizeGoal:
        case GoalID::ParkValueGoal:
        case GoalID::ParkRatingGoal:
            goal->SetSign(static_cast<Sign>(dropdownIndex + 1));
            break;

        case GoalID::SpecificTrackedRideGoal:
            if (widgetIndex == WIDX_BUILD_TRACK_ADD_PIECE_DROPDOWN)
            {
                std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->AddTrackPieceRequired(
                    dropDownPieces[dropdownIndex]);
            }
            [[fallthrough]];
        case GoalID::ResearchCertainRide:
        case GoalID::SpecificRideGoal:
            if (widgetIndex == WIDX_BUILD_RIDE_DROPDOWN)
            {
                std::static_pointer_cast<ObjectiveResearchRide>(goal)->SetBaseRideType(dropDownRides[dropdownIndex]);
                std::static_pointer_cast<ObjectiveResearchRide>(goal)->SetEntryIndex(dropDownRides2[dropdownIndex]);
            }
            break;
        case GoalID::NoDuplicateRidesGoal:
            if (widgetIndex == WIDX_AWARD_DROPDOWN)
                std::static_pointer_cast<ObjectiveNoDuplicateRidesGoal>(goal)->SetCategory(dropdownIndex);
            break;
    }
    w->Invalidate();
}

static void WindowEditorObjectiveOptionsAddObjectiveTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (text == nullptr)
        return;

    uint32_t castedValue = 0;
    uint32_t maxVal = 2;
    uint32_t minVal = 0;
    uint32_t valMin = 0;
    bool error = false;
    bool max = false;
    switch (goalID)
    {
        case GoalID::FoodProfitGoal:
            maxVal = 100000;
            break;
        case GoalID::GuestNumGoal:
            minVal = 1;
            maxVal = 60000;
            break;
        case GoalID::ProfitGoal:
            maxVal = 1000000;
            break;
        case GoalID::MerchandiseProfitGoal:
            maxVal = 100000;
            break;
        case GoalID::ParkEntryProfitGoal:
            maxVal = 500000;
            break;
        case GoalID::ParkRatingGoal:
            minVal = 1;
            maxVal = 999;
            break;
        case GoalID::ParkValueGoal:
            maxVal = 50000000;
            break;
        case GoalID::ParkSizeGoal:
            minVal = 10;
            maxVal = 1000000;
            break;
        case GoalID::RideTicketProfitGoal:
            maxVal = 500000;
            break;
        case GoalID::StallProfitGoal:
            maxVal = 200000;
            break;
        case GoalID::CoasterGoal:
        case GoalID::WaterRidesGoal:
        case GoalID::TransportRidesGoal:
        case GoalID::GentleRidesGoal:
        case GoalID::ThrillRidesGoal:
            switch (widgetIndex)
            {
                case WIDX_RIDE_NUM_MAX:
                    minVal = 0;
                    [[fallthrough]];
                case WIDX_RIDE_NUM_MIN:
                    maxVal = 50;
                    break;
                case WIDX_RIDE_LENGTH_MAX:
                    max = true;
                    if (gConfigGeneral.measurement_format != MeasurementFormat::Imperial)
                        valMin = std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal();
                    else
                        valMin = metres_to_feet(
                            std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal());
                    [[fallthrough]];
                case WIDX_RIDE_LENGTH_MIN:
                    minVal = 0;
                    maxVal = 20000;
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                        maxVal = 65617;
                    break;
                case WIDX_RIDE_EXCITEMENT_MAX:
                    max = true;
                    valMin = std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->GetMinRideExcitementGoal();
                    [[fallthrough]];
                case WIDX_RIDE_EXCITEMENT_MIN:
                    minVal = 0;
                    maxVal = 1500;
                    break;
                case WIDX_RIDE_INTENSITY_MAX:
                    valMin = std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMinRideIntensityGoal();
                    max = true;
                    minVal = 0;
                    maxVal = 1200;
                    break;
                case WIDX_RIDE_NAUSEA_MAX:
                    valMin = std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMinRideNauseaGoal();
                    max = true;
                    break;
                    [[fallthrough]];
                case WIDX_RIDE_INTENSITY_MIN:
                case WIDX_RIDE_NAUSEA_MIN:
                    minVal = 0;
                    maxVal = 1200;
                    break;
            }
            break;
        case GoalID::SpecificTrackedRideGoal:
            minVal = 0;
            switch (widgetIndex)
            {
                case WIDX_BUILD_TRACK_HEIGHT_MAX:
                    max = true;
                    if (gConfigGeneral.measurement_format != MeasurementFormat::Imperial)
                        valMin = std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinDropHeightGoal();
                    else
                        valMin = metres_to_feet(
                            std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinDropHeightGoal());
                    [[fallthrough]];
                case WIDX_BUILD_TRACK_HEIGHT_MIN:
                    maxVal = 180;
                    break;
                case WIDX_BUILD_TRACK_LENGTH_MAX:
                    max = true;
                    if (gConfigGeneral.measurement_format != MeasurementFormat::Imperial)
                        valMin = std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinLengthGoal();
                    else
                        valMin = metres_to_feet(
                            std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinLengthGoal());
                    [[fallthrough]];
                case WIDX_BUILD_TRACK_LENGTH_MIN:
                    maxVal = 35000;
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                    {
                        maxVal = 100000;
                    }
                    break;
                case WIDX_BUILD_TRACK_EXCITEMENT_MAX:
                    max = true;
                    valMin = std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinExcitementGoal();
                    maxVal = 2000;
                    break;
                case WIDX_BUILD_TRACK_INTENSITY_MAX:
                    max = true;
                    valMin = std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinIntensityGoal();
                    maxVal = 2000;
                    break;
                case WIDX_BUILD_TRACK_NAUSEA_MAX:
                    max = true;
                    valMin = std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinNauseaGoal();
                    [[fallthrough]];
                case WIDX_BUILD_TRACK_EXCITEMENT_MIN:
                case WIDX_BUILD_TRACK_INTENSITY_MIN:
                case WIDX_BUILD_TRACK_NAUSEA_MIN:
                    maxVal = 2000;
                    break;
                case WIDX_BUILD_TRACK_SPEED_MAX:
                    max = true;
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                        valMin = std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinSpeedGoal();
                    else
                        valMin = mph_to_kmph(
                            std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinSpeedGoal());
                    [[fallthrough]];
                case WIDX_BUILD_TRACK_SPEED_MIN:
                    maxVal = 999;
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                    {
                        maxVal = 621;
                    }
                    break;
                case WIDX_BUILD_TRACK_DROPS_MAX:
                    max = true;
                    valMin = std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinDropCountGoal();
                    maxVal = 32;
                    break;
                case WIDX_BUILD_TRACK_INVERSIONS_MAX:
                    valMin = std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinNumInversions();
                    max = true;
                    [[fallthrough]];
                case WIDX_BUILD_TRACK_DROPS_MIN:
                case WIDX_BUILD_TRACK_INVERSIONS_MIN:
                    maxVal = 31;
                    break;
            }
            break;
        default:
            maxVal = 1000;
            break;
    }
    try
    {
        std::string::size_type sz;
        castedValue = std::stoi(text, &sz);
        std::string s = std::string(text);
        if (s.size() != sz)
            error = true;
    }
    catch (std::exception const&)
    {
        error = true;
    }

    if (castedValue < minVal || castedValue > maxVal)
        error = true;

    if (error)
    {
        auto ft = Formatter();
        if (max && minVal != valMin)
        {
            ft.Add<uint32_t>(valMin);
            ft.Add<uint32_t>(maxVal);
            context_show_error(STR_ERROR_ENTER_MAX_VALUE, STR_NONE, ft);
        }
        else
        {
            ft.Add<uint32_t>(minVal);
            ft.Add<uint32_t>(maxVal);
            context_show_error(STR_ERROR_ENTER_VALUE_BETWEEN, STR_NONE, ft);
        }
        return;
    }

    switch (goalID)
    {
        case GoalID::CoasterGoal:
        case GoalID::WaterRidesGoal:
        case GoalID::TransportRidesGoal:
        case GoalID::GentleRidesGoal:
        case GoalID::ThrillRidesGoal:
            switch (widgetIndex)
            {
                case WIDX_RIDE_NUM_MIN:
                    std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->SetMinNumRidesGoal(
                        static_cast<uint8_t>(castedValue));
                    safe_strcpy(textbox1Buffer, text, sizeof(textbox1Buffer));
                    break;
                case WIDX_RIDE_NUM_MAX:
                    std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->SetMaxNumRidesGoal(
                        static_cast<uint8_t>(castedValue));
                    safe_strcpy(textbox2Buffer, text, sizeof(textbox2Buffer));
                    break;
                case WIDX_RIDE_LENGTH_MIN:
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                        std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->SetMinRideLengthGoal(
                            static_cast<uint16_t>(feet_to_metres(castedValue)));
                    else
                        std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->SetMinRideLengthGoal(
                            static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox3Buffer, text, sizeof(textbox3Buffer));

                    break;
                case WIDX_RIDE_LENGTH_MAX:
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                        std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->SetMaxRideLengthGoal(
                            static_cast<uint16_t>(feet_to_metres(castedValue)));
                    else
                        std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->SetMaxRideLengthGoal(
                            static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox4Buffer, text, sizeof(textbox4Buffer));
                    break;
                case WIDX_RIDE_EXCITEMENT_MIN:
                    std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->SetMinRideExcitementGoal(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox5Buffer, text, sizeof(textbox5Buffer));
                    break;
                case WIDX_RIDE_EXCITEMENT_MAX:
                    std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->SetMaxRideExcitementGoal(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox6Buffer, text, sizeof(textbox6Buffer));
                    break;
                case WIDX_RIDE_INTENSITY_MIN:
                    std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->SetMinRideIntensityGoal(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox7Buffer, text, sizeof(textbox7Buffer));
                    break;
                case WIDX_RIDE_INTENSITY_MAX:
                    std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->SetMaxRideIntensityGoal(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox8Buffer, text, sizeof(textbox8Buffer));
                    break;
                case WIDX_RIDE_NAUSEA_MIN:
                    std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->SetMinRideNauseaGoal(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox9Buffer, text, sizeof(textbox9Buffer));
                    break;
                case WIDX_RIDE_NAUSEA_MAX:
                    std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->SetMaxRideNauseaGoal(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox10Buffer, text, sizeof(textbox10Buffer));
                    break;
            }

            w->Invalidate();
            break;
        case GoalID::SpecificTrackedRideGoal:
        {
            switch (widgetIndex)
            {
                case WIDX_BUILD_TRACK_DROPS_MIN:
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinDropCountGoal(
                        static_cast<uint8_t>(castedValue));
                    safe_strcpy(textbox1Buffer, text, sizeof(textbox1Buffer));
                    break;
                case WIDX_BUILD_TRACK_DROPS_MAX:
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxDropCountGoal(
                        static_cast<uint8_t>(castedValue));
                    safe_strcpy(textbox2Buffer, text, sizeof(textbox2Buffer));
                    break;
                case WIDX_BUILD_TRACK_HEIGHT_MIN:
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinDropHeightGoal(
                            static_cast<uint16_t>(feet_to_metres(castedValue)));
                    else
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinDropHeightGoal(
                            static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox3Buffer, text, sizeof(textbox3Buffer));
                    break;
                case WIDX_BUILD_TRACK_HEIGHT_MAX:
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxDropHeightGoal(
                            static_cast<uint16_t>(feet_to_metres(castedValue)));
                    else
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxDropHeightGoal(
                            static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox4Buffer, text, sizeof(textbox4Buffer));
                    break;
                case WIDX_BUILD_TRACK_LENGTH_MIN:
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinLengthGoal(
                            static_cast<uint16_t>(feet_to_metres(castedValue)));
                    else
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinLengthGoal(
                            static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox5Buffer, text, sizeof(textbox5Buffer));
                    break;
                case WIDX_BUILD_TRACK_LENGTH_MAX:
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxLengthGoal(
                            static_cast<uint16_t>(feet_to_metres(castedValue)));
                    else
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxLengthGoal(
                            static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox6Buffer, text, sizeof(textbox6Buffer));
                    break;
                case WIDX_BUILD_TRACK_EXCITEMENT_MIN:
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinExcitementGoal(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox7Buffer, text, sizeof(textbox7Buffer));
                    break;
                case WIDX_BUILD_TRACK_EXCITEMENT_MAX:
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxExcitementGoal(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox8Buffer, text, sizeof(textbox8Buffer));
                    break;
                case WIDX_BUILD_TRACK_INTENSITY_MIN:
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinIntensityGoal(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox9Buffer, text, sizeof(textbox9Buffer));
                    break;
                case WIDX_BUILD_TRACK_INTENSITY_MAX:
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxIntensityGoal(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox10Buffer, text, sizeof(textbox10Buffer));
                    break;
                case WIDX_BUILD_TRACK_NAUSEA_MIN:
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinNauseaGoal(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox11Buffer, text, sizeof(textbox11Buffer));
                    break;
                case WIDX_BUILD_TRACK_NAUSEA_MAX:
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxNauseaGoal(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox12Buffer, text, sizeof(textbox12Buffer));
                    break;
                case WIDX_BUILD_TRACK_SPEED_MIN:
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Metric)
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinSpeedGoal(
                            static_cast<uint16_t>(kmph_to_mph(castedValue)));
                    else if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinSpeedGoal(
                            static_cast<uint16_t>(castedValue));
                    else
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinSpeedGoal(
                            static_cast<uint16_t>(kmph_to_mph(mph_to_dmps(castedValue))));
                    safe_strcpy(textbox13Buffer, text, sizeof(textbox13Buffer));
                    break;
                case WIDX_BUILD_TRACK_SPEED_MAX:
                    if (gConfigGeneral.measurement_format == MeasurementFormat::Metric)
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxSpeedGoal(
                            static_cast<uint16_t>(kmph_to_mph(castedValue)));
                    else if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxSpeedGoal(
                            static_cast<uint16_t>(castedValue));
                    else
                        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxSpeedGoal(
                            static_cast<uint16_t>(kmph_to_mph(mph_to_dmps(castedValue))));
                    safe_strcpy(textbox14Buffer, text, sizeof(textbox14Buffer));
                    break;
                case WIDX_BUILD_TRACK_INVERSIONS_MIN:
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinNumInversions(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox15Buffer, text, sizeof(textbox15Buffer));
                    break;
                case WIDX_BUILD_TRACK_INVERSIONS_MAX:
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxNumInversions(
                        static_cast<uint16_t>(castedValue));
                    safe_strcpy(textbox16Buffer, text, sizeof(textbox16Buffer));
                    break;
            }
            break;
        }
        default:
        {
            if (widgetIndex == WIDX_VALUE_SET_AMOUNT)
            {
                switch (goalID)
                {
                    case GoalID::FoodProfitGoal:
                    case GoalID::ProfitGoal:
                    case GoalID::MerchandiseProfitGoal:
                    case GoalID::ParkEntryProfitGoal:
                    case GoalID::StallProfitGoal:
                    case GoalID::RideTicketProfitGoal:
                        std::static_pointer_cast<ObjectiveProfitGoal>(goal)->SetProfitGoal(castedValue * 10);
                        break;
                    case GoalID::ParkRatingGoal:
                        std::static_pointer_cast<ObjectiveParkRatingGoal>(goal)->SetParkRatingGoal(castedValue);
                        break;
                    case GoalID::ParkValueGoal:
                        std::static_pointer_cast<ObjectiveParkValueGoal>(goal)->SetParkValueGoal(castedValue * 10);
                        break;
                    case GoalID::ParkSizeGoal:
                        std::static_pointer_cast<ObjectiveParkSizeGoal>(goal)->SetParkSizeGoal(castedValue / 10);
                        break;
                    case GoalID::GuestNumGoal:
                        std::static_pointer_cast<ObjectiveGuestNumGoal>(goal)->SetGuestNumGoal(castedValue);
                        break;
                }
            }
        }
    }
}

static void WindowEditorObjectiveOptionsAddObjectiveInvalidate(rct_window* w)
{
    ColourSchemeUpdateByClass(w, static_cast<rct_windowclass>(WC_EDITOR_OBJECTIVE_OPTIONS));
    GoalID g = goal->GetGoalID();

    switch (g)
    {
        case GoalID::AwardGoal:
            if (!std::static_pointer_cast<ObjectiveAwardGoal>(goal)->GetAtAnyTime())
                w->pressed_widgets |= (1ULL << WIDX_AWARD_ANYTIME);
            else
                w->pressed_widgets &= ~(1ULL << WIDX_AWARD_ANYTIME);
            break;
        case GoalID::ThrillRidesGoal:
        case GoalID::GentleRidesGoal:
        case GoalID::CoasterGoal:
        case GoalID::TransportRidesGoal:
        case GoalID::WaterRidesGoal:
            if (std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMustBeUniqueTypes())
                w->pressed_widgets |= (1ULL << WIDX_RIDE_UNIQUE);
            else
                w->pressed_widgets &= ~(1ULL << WIDX_RIDE_UNIQUE);

            if (g != GoalID::ThrillRidesGoal)
            {
                if (std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetFinishedExistingRides())
                    w->pressed_widgets |= (1ULL << WIDX_RIDE_FINISHED_RIDES);
                else
                    w->pressed_widgets &= ~(1ULL << WIDX_RIDE_FINISHED_RIDES);
            }
            break;

        case GoalID::FoodProfitGoal:
        case GoalID::GuestNumGoal:
        case GoalID::ProfitGoal:
        case GoalID::MerchandiseProfitGoal:
        case GoalID::ParkEntryProfitGoal:
        case GoalID::ParkRatingGoal:
        case GoalID::ParkValueGoal:
        case GoalID::RideTicketProfitGoal:
        case GoalID::StallProfitGoal: // Not for parksize.
            if (goal->GetGoalType() == GoalType::Restriction)
            {
                w->pressed_widgets |= (1ULL << WIDX_VALUE_RESTRICTION);
                window_editor_objective_options_value_sign_goal_widets[WIDX_VALUE_WARNING_WEEKS].type = WindowWidgetType::
                    Spinner;
                window_editor_objective_options_value_sign_goal_widets[WIDX_VALUE_WARNING_WEEKS_DECREASE].type
                    = WindowWidgetType::Button;
                window_editor_objective_options_value_sign_goal_widets[WIDX_VALUE_WARNING_WEEKS_INCREASE].type
                    = WindowWidgetType::Button;
            }
            else
            {
                w->pressed_widgets &= ~(1ULL << WIDX_VALUE_RESTRICTION);
                window_editor_objective_options_value_sign_goal_widets[WIDX_VALUE_WARNING_WEEKS].type = WindowWidgetType::Empty;
                window_editor_objective_options_value_sign_goal_widets[WIDX_VALUE_WARNING_WEEKS_DECREASE].type
                    = WindowWidgetType::Empty;
                window_editor_objective_options_value_sign_goal_widets[WIDX_VALUE_WARNING_WEEKS_INCREASE].type
                    = WindowWidgetType::Empty;
            }
            break;
        case GoalID::SpecificRideGoal:
        case GoalID::SpecificTrackedRideGoal:
            if (std::static_pointer_cast<ObjectiveSpecificRideGoal>(goal)->GetMustBeOnlySuchRide())
                w->pressed_widgets |= (1ULL << WIDX_BUILD_RIDE_UNIQUE);
            else
                w->pressed_widgets &= ~(1ULL << WIDX_BUILD_RIDE_UNIQUE);

            if (g == GoalID::SpecificRideGoal)
            {
                if (std::static_pointer_cast<ObjectiveSpecificRideGoal>(goal)->GetBuildBestGoal())
                {
                    w->pressed_widgets |= (1ULL << WIDX_BUILD_RIDE_BEST);
                    window_editor_objective_options_specific_ride_goal_widets[WIDX_BUILD_RIDE_UNIQUE].type = WindowWidgetType::
                        Empty;
                }
                else
                {
                    w->pressed_widgets &= ~(1ULL << WIDX_BUILD_RIDE_BEST);
                    window_editor_objective_options_specific_ride_goal_widets[WIDX_BUILD_RIDE_UNIQUE].type = WindowWidgetType::
                        Checkbox;
                }
            }
            else // if tracked
            {
                uint8_t baseRide = std::static_pointer_cast<ObjectiveSpecificRideGoal>(goal)->GetBaseRideType();
                if (GetRideTypeDescriptor(baseRide).HasFlag(RIDE_TYPE_FLAG_HAS_DROPS))
                {
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_DROPS_MIN].type
                        = WindowWidgetType::TextBox;
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_DROPS_MAX].type
                        = WindowWidgetType::TextBox;
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_HEIGHT_MIN].type
                        = WindowWidgetType::TextBox;
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_HEIGHT_MAX].type
                        = WindowWidgetType::TextBox;
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_INVERSIONS_MIN].type
                        = WindowWidgetType::TextBox;
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_INVERSIONS_MAX].type
                        = WindowWidgetType::TextBox;
                }
                else if (
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_DROPS_MIN].type
                    != WindowWidgetType::Empty) // prevent it constantly setting
                {
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_DROPS_MIN].type
                        = WindowWidgetType::Empty;
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_DROPS_MAX].type
                        = WindowWidgetType::Empty;
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_HEIGHT_MIN].type
                        = WindowWidgetType::Empty;
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_HEIGHT_MAX].type
                        = WindowWidgetType::Empty;
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_INVERSIONS_MIN].type
                        = WindowWidgetType::Empty;
                    window_editor_objective_options_tracked_ride_goal_widets[WIDX_BUILD_TRACK_INVERSIONS_MAX].type
                        = WindowWidgetType::Empty;

                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinDropCountGoal(0);
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxDropCountGoal(0);
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinDropHeightGoal(0);
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxDropHeightGoal(0);
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMinNumInversions(0);
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->SetMaxNumInversions(0);

                    safe_strcpy(textbox1Buffer, "0", sizeof(textbox1Buffer));
                    safe_strcpy(textbox2Buffer, "0", sizeof(textbox2Buffer));
                    safe_strcpy(textbox3Buffer, "0", sizeof(textbox3Buffer));
                    safe_strcpy(textbox4Buffer, "0", sizeof(textbox4Buffer));
                    safe_strcpy(textbox15Buffer, "0", sizeof(textbox15Buffer));
                    safe_strcpy(textbox16Buffer, "0", sizeof(textbox16Buffer));
                }

                if (deleteTrackPiecesMode)
                    w->pressed_widgets |= (1ULL << WIDX_BUILD_TRACK_REMOVE_PIECES);
                else
                    w->pressed_widgets &= ~(1ULL << WIDX_BUILD_TRACK_REMOVE_PIECES);
            }
            break;
    }
    WindowEditorObjectiveOptionsAddObjectiveAnchorBorderWidgets(w);
}

static void WindowEditorObjectiveOptionsAddObjectiveSignDropdown(rct_window* w)
{
    rct_widget* dropdownWidget;

    dropdownWidget = &w->widgets[WIDX_VALUE_SIGN];

    gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
    gDropdownItems[0].Args = STR_AT_LEAST;
    gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
    gDropdownItems[1].Args = STR_AT_MOST;

    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, Dropdown::Flag::StayOpen, 2, dropdownWidget->width() - 3);
    Dropdown::SetChecked((int32_t)goal->GetSign() - 1, true);
}

static void WindowEditorObjectiveOptionsAddObjectiveAwardDropdown(rct_window* w)
{
    int32_t i;
    rct_widget* dropdownWidget;

    dropdownWidget = &w->widgets[WIDX_AWARD];
    for (i = 0; i < 17; i++)
    {
        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
        gDropdownItems[i].Args = STR_AWARD_MOST_UNTIDY_COLOURLESS + i;
    }
    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, Dropdown::Flag::StayOpen, 17, dropdownWidget->width() - 3);
    Dropdown::SetChecked((int32_t)std::static_pointer_cast<ObjectiveAwardGoal>(goal)->GetAwardsGoal(), true);
}

static void WindowEditorObjectiveOptionsAddObjectiveRideCategoryDropdown(rct_window* w)
{
    int32_t i;
    rct_widget* dropdownWidget;

    dropdownWidget = &w->widgets[WIDX_AWARD];
    for (i = 0; i < 6; i++)
    {
        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
        gDropdownItems[i].Args = STR_RESEARCH_CATEGORY_TRANSPORT + i;
    }
    gDropdownItems[6].Format = STR_DROPDOWN_MENU_LABEL;
    gDropdownItems[6].Args = STR_ALL_RIDES;
    gDropdownItems[7].Format = STR_DROPDOWN_MENU_LABEL;
    gDropdownItems[7].Args = STR_ALL_RIDES_AND_STALLS;

    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, Dropdown::Flag::StayOpen, 8, dropdownWidget->width() - 3);
    Dropdown::SetChecked((int32_t)std::static_pointer_cast<ObjectiveNoDuplicateRidesGoal>(goal)->GetCategory(), true);
}

rct_string_id GetRideName(uint8_t baseRideType, rct_ride_entry* rideEntry)
{
    if (!GetRideTypeDescriptor(baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
        return get_ride_naming(baseRideType, rideEntry).Name;
    else if (rideEntry != nullptr)
        return rideEntry->naming.Name;
    else
        return GetRideTypeDescriptor(baseRideType).Naming.Name;
}

static void WindowEditorObjectiveOptionsAddObjectiveRideDropdown(rct_window* w)
{
    int32_t i = 0, checkedIndex = 0;
    rct_widget* widget;
    dropDownRides = std::vector<uint8_t>();
    dropDownRides2 = std::vector<uint16_t>();
    dropDownRideTypes = std::vector<rct_string_id>();
    switch (goalID)
    {
        case GoalID::ResearchCertainRide:
        {
            widget = &w->widgets[WIDX_BUILD_RIDE];
            for (auto item : gResearchItemsUninvented)
            {
                if (item.type == Research::EntryType::Ride)
                {
                    gDropdownItems[i].Args = GetRideName(item.baseRideType, get_ride_entry(item.entryIndex));
                    dropDownRides.push_back(item.baseRideType);
                    dropDownRides2.push_back(item.entryIndex);
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    if (item.entryIndex == std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetEntryIndex()
                        && item.baseRideType == std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetBaseRideType())
                        checkedIndex = i;
                    i++;
                }
            }
        }
        break;
        case GoalID::SpecificRideGoal:
        {
            bool skip = false;
            widget = &w->widgets[WIDX_BUILD_RIDE];
            for (auto item : gResearchItemsUninvented)
            {
                if (item.type == Research::EntryType::Ride)
                {
                    skip = false;
                    rct_string_id str = GetRideName(item.baseRideType, get_ride_entry(item.entryIndex));
                    for (auto listed : dropDownRideTypes)
                    {
                        if (skip)
                            continue;
                        if (listed == str)
                            skip = true;
                    }
                    if (skip)
                        continue;
                    dropDownRideTypes.push_back(str);
                    gDropdownItems[i].Args = str;
                    dropDownRides.push_back(item.baseRideType);
                    dropDownRides2.push_back(item.entryIndex);
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    if (item.entryIndex == std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetEntryIndex()
                        && item.baseRideType == std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetBaseRideType())
                        checkedIndex = i;
                    i++;
                }
            }
            for (auto item : gResearchItemsInvented)
            {
                if (item.type == Research::EntryType::Ride)
                {
                    skip = false;
                    rct_string_id str = GetRideName(item.baseRideType, get_ride_entry(item.entryIndex));
                    for (auto listed : dropDownRideTypes)
                    {
                        if (skip)
                            continue;
                        if (listed == str)
                            skip = true;
                    }
                    if (skip)
                        continue;
                    dropDownRideTypes.push_back(str);
                    gDropdownItems[i].Args = str;
                    dropDownRides.push_back(item.baseRideType);
                    dropDownRides2.push_back(item.entryIndex);
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    if (item.entryIndex == std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetEntryIndex()
                        && item.baseRideType == std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetBaseRideType())
                        checkedIndex = i;
                    i++;
                }
            }
        }
        break;
        case GoalID::SpecificTrackedRideGoal:
        default:
        {
            bool skip = false;
            widget = &w->widgets[WIDX_BUILD_TRACK];
            for (auto item : gResearchItemsUninvented)
            {
                if (item.type == Research::EntryType::Ride
                    && !GetRideTypeDescriptor(item.baseRideType).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
                {
                    skip = false;
                    rct_string_id str = GetRideName(item.baseRideType, get_ride_entry(item.entryIndex));
                    for (auto listed : dropDownRideTypes)
                    {
                        if (skip)
                            continue;
                        if (listed == str)
                            skip = true;
                    }
                    if (skip)
                        continue;
                    dropDownRideTypes.push_back(str);
                    gDropdownItems[i].Args = str;
                    dropDownRides.push_back(item.baseRideType);
                    dropDownRides2.push_back(item.entryIndex);
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    if (item.entryIndex == std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetEntryIndex()
                        && item.baseRideType == std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetBaseRideType())
                        checkedIndex = i;
                    i++;
                }
            }
            for (auto item : gResearchItemsInvented)
            {
                if (item.type == Research::EntryType::Ride
                    && !GetRideTypeDescriptor(item.baseRideType).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
                {
                    skip = false;
                    rct_string_id str = GetRideName(item.baseRideType, get_ride_entry(item.entryIndex));
                    for (auto listed : dropDownRideTypes)
                    {
                        if (skip)
                            continue;
                        if (listed == str)
                            skip = true;
                    }
                    if (skip)
                        continue;
                    dropDownRideTypes.push_back(str);
                    gDropdownItems[i].Args = str;
                    dropDownRides.push_back(item.baseRideType);
                    dropDownRides2.push_back(item.entryIndex);
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    if (item.entryIndex == std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetEntryIndex()
                        && item.baseRideType == std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetBaseRideType())
                        checkedIndex = i;
                    i++;
                }
            }
        }
        break;
    }
    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0,
        Dropdown::Flag::StayOpen, i, widget->width() - 3);

    Dropdown::SetChecked(checkedIndex, true);
}

static void WindowEditorObjectiveOptionsAddObjectiveAddPieceDropdown(rct_window* w)
{
    int i = 0, j = 0;
    dropDownPieces = std::vector<uint16_t>();

    rct_widget* dropdownWidget;
    dropdownWidget = &w->widgets[WIDX_BUILD_TRACK_ADD_PIECE];
    for (auto item : RideConfigurationStringIds)
    {
        j++;
        if (item == STR_EMPTY) // ignore normal track
            continue;
        auto types = TrackNameToTypes.at(item);
        bool skip = true;
        for (auto type : types)
        {
            if (GetRideTypeDescriptor(std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetBaseRideType())
                    .SupportsTrackPiece(type))
                skip = false;
        }
        if (skip)
            continue;
        // check against duplicate names (to not have each quarter loop as seperate selectable option for example)
        for (auto listed : dropDownPieces)
        {
            if (skip)
                continue;
            if (RideConfigurationStringIds[listed] == item)
                skip = true;
        }
        if (skip)
            continue;

        gDropdownItems[i].Args = item;
        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
        dropDownPieces.push_back(j - 1);
        i++;
    }
    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + dropdownWidget->left, w->windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
        w->colours[1], 0, Dropdown::Flag::StayOpen, i, dropdownWidget->width() - 3);
}

static void WindowEditorObjectiveOptionsAddObjectiveMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (goal->GetGoalID())
    {
        case GoalID::AwardGoal:
            if (widgetIndex == WIDX_AWARD_DROPDOWN)
                WindowEditorObjectiveOptionsAddObjectiveAwardDropdown(w);
            break;
        case GoalID::ProfitGoal:
        case GoalID::ParkEntryProfitGoal:
        case GoalID::RideTicketProfitGoal:
        case GoalID::FoodProfitGoal:
        case GoalID::MerchandiseProfitGoal:
        case GoalID::StallProfitGoal:
        case GoalID::GuestNumGoal:
        case GoalID::ParkSizeGoal:
        case GoalID::ParkValueGoal:
        case GoalID::ParkRatingGoal:
            if (widgetIndex == WIDX_VALUE_SIGN_DROPDOWN)
                WindowEditorObjectiveOptionsAddObjectiveSignDropdown(w);
            else if (widgetIndex == WIDX_VALUE_WARNING_WEEKS_DECREASE)
                goal->SetWarningWeeksPeriod(std::clamp<int8_t>(goal->GetWarningWeeksPeriod() - 1, 1, 64));
            else if (widgetIndex == WIDX_VALUE_WARNING_WEEKS_INCREASE)
                goal->SetWarningWeeksPeriod(std::clamp<int8_t>(goal->GetWarningWeeksPeriod() + 1, 1, 64));
            break;
        case GoalID::NumPositiveAwardsGoal:
            if (widgetIndex == WIDX_NUM_AWARDS_INCREASE)
                std::static_pointer_cast<ObjectiveNumPositiveAwardsGoal>(goal)->SetNumAwards(std::clamp<int16_t>(
                    std::static_pointer_cast<ObjectiveNumPositiveAwardsGoal>(goal)->GetNumAwards() + 1, 1, 12));
            else if (widgetIndex == WIDX_NUM_AWARDS_DECREASE)
                std::static_pointer_cast<ObjectiveNumPositiveAwardsGoal>(goal)->SetNumAwards(std::clamp<int16_t>(
                    std::static_pointer_cast<ObjectiveNumPositiveAwardsGoal>(goal)->GetNumAwards() - 1, 1, 12));
            break;
        case GoalID::SpecificTrackedRideGoal:
            if (widgetIndex == WIDX_BUILD_TRACK_ADD_PIECE_DROPDOWN)
                WindowEditorObjectiveOptionsAddObjectiveAddPieceDropdown(w);
            [[fallthrough]];
        case GoalID::ResearchCertainRide:
        case GoalID::SpecificRideGoal:
            if (widgetIndex == WIDX_BUILD_RIDE_DROPDOWN)
                WindowEditorObjectiveOptionsAddObjectiveRideDropdown(w);
            break;
        case GoalID::NoDuplicateRidesGoal:
            if (widgetIndex == WIDX_AWARD_DROPDOWN)
                WindowEditorObjectiveOptionsAddObjectiveRideCategoryDropdown(w);
    }
}

template<typename statType>
static void DrawStat(
    rct_window* w, rct_drawpixelinfo* dpi, uint8_t minWidget, uint8_t maxWidget, rct_string_id minLabel, rct_string_id maxLabel,
    statType minVal, statType maxVal, rct_string_id valueString, bool margin = false)
{
    auto screenCoords = w->windowPos + ScreenCoordsXY{ w->min_width / 2, 25 };
    auto ft = Formatter();

    // select min label
    screenCoords = w->windowPos + ScreenCoordsXY{ 8 + margin, w->widgets[minWidget].top };
    ft = Formatter();
    ft.Add<rct_string_id>(minLabel);
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

    // left in case this is the way to go for getting values back in the textfields, with meters, or 10.00 ratings.
    // value
    /*screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[minWidget].left + 1, w->widgets[minWidget].top };
    ft = Formatter();
    ft.Add<statType>(minVal);
    DrawTextEllipsised(dpi, screenCoords, w->widgets[minWidget].width(), valueString, ft);*/

    // select max label
    screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[maxWidget].top };
    if (margin)
        screenCoords += ScreenCoordsXY{ 297, 0 };
    ft = Formatter();
    ft.Add<rct_string_id>(maxLabel);
    DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

    // value
    /*screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[maxWidget].left + 1, w->widgets[maxWidget].top };
    ft = Formatter();
    ft.Add<statType>(maxVal);
    DrawTextEllipsised(dpi, screenCoords, w->widgets[maxWidget].width(), valueString, ft);*/
}

static void WindowEditorObjectiveOptionsAddObjectivePaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    auto screenCoords = w->windowPos + ScreenCoordsXY{ w->min_width / 2, 25 };
    auto ft = Formatter();
    ft.Add<rct_string_id>(STR_STRING);
    std::string s = goal->ToString();
    ft.Add<const char*>(s.c_str());
    DrawTextWrapped(
        dpi, screenCoords, w->min_width - 20, STR_WINDOW_COLOUR_2_STRINGID, ft, { TextAlignment::CENTRE_HORIZONTAL_ONLY });
    switch (goalID)
    {
        case GoalID::AwardGoal:
            // select award label
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_AWARD].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_SELECT_AWARD);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

            // Award value
            screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_AWARD].left + 1, w->widgets[WIDX_AWARD].top };
            ft = Formatter();
            ft.Add<rct_string_id>(
                STR_AWARD_MOST_UNTIDY + (uint32_t)std::static_pointer_cast<ObjectiveAwardGoal>(goal)->GetAwardsGoal());
            DrawTextEllipsised(dpi, screenCoords, w->widgets[WIDX_AWARD].width(), STR_WINDOW_COLOUR_2_STRINGID, ft);

            // Award anytime label
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_AWARD_ANYTIME].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_AWARD_ANYTIME);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            break;
        case GoalID::CoasterGoal:
            DrawStat<uint32_t>(
                w, dpi, WIDX_RIDE_INTENSITY_MIN, WIDX_RIDE_INTENSITY_MAX, STR_MIN_INTENSITY_RATING, STR_MAX_INTENSITY_RATING,
                std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMinRideIntensityGoal(),
                std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMaxRideIntensityGoal(), STR_EX_IN_NAU_RATING);
            DrawStat<uint32_t>(
                w, dpi, WIDX_RIDE_NAUSEA_MIN, WIDX_RIDE_NAUSEA_MAX, STR_MIN_NAUSEA_RATING, STR_MAX_NAUSEA_RATING,
                std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMinRideNauseaGoal(),
                std::static_pointer_cast<ObjectiveCoasterGoal>(goal)->GetMaxRideNauseaGoal(), STR_EX_IN_NAU_RATING);
            [[fallthrough]];
        case GoalID::WaterRidesGoal:
            DrawStat<uint32_t>(
                w, dpi, WIDX_RIDE_EXCITEMENT_MIN, WIDX_RIDE_EXCITEMENT_MAX, STR_MIN_EXCITEMENT_RATING,
                STR_MAX_EXCITEMENT_RATING, std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->GetMinRideExcitementGoal(),
                std::static_pointer_cast<ObjectiveWaterRidesGoal>(goal)->GetMaxRideExcitementGoal(), STR_EX_IN_NAU_RATING);
            [[fallthrough]];
        case GoalID::TransportRidesGoal:
            DrawStat<uint16_t>(
                w, dpi, WIDX_RIDE_LENGTH_MIN, WIDX_RIDE_LENGTH_MAX, STR_MIN_TRACK_LENGTH, STR_MAX_TRACK_LENGTH,
                std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMinRideLengthGoal(),
                std::static_pointer_cast<ObjectiveTransportRidesGoal>(goal)->GetMaxRideLengthGoal(), STR_RIDE_LENGTH_ENTRY);
            [[fallthrough]];
        case GoalID::GentleRidesGoal:
            // select finish rides label
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_RIDE_FINISHED_RIDES].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_FINISH_RIDES);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            [[fallthrough]];
        case GoalID::ThrillRidesGoal:
            DrawStat<uint16_t>(
                w, dpi, WIDX_RIDE_NUM_MIN, WIDX_RIDE_NUM_MAX, STR_MIN_RIDE_NUM, STR_MAX_RIDE_NUM,
                std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMinNumRidesGoal(),
                std::static_pointer_cast<ObjectiveThrillRidesGoal>(goal)->GetMaxNumRidesGoal(), STR_FORMAT_INTEGER);
            // select unique rides label
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_RIDE_UNIQUE].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_UNIQUE_RIDES);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            break;

        case GoalID::ProfitGoal:
        case GoalID::FoodProfitGoal:
        case GoalID::MerchandiseProfitGoal:
        case GoalID::ParkEntryProfitGoal:
        case GoalID::RideTicketProfitGoal:
        case GoalID::StallProfitGoal:
        case GoalID::GuestNumGoal:
        case GoalID::ParkRatingGoal:
        case GoalID::ParkValueGoal:
            // Restriction label
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_VALUE_RESTRICTION].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_IS_RESTRICTION);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

            if (goal->GetGoalType() == GoalType::Restriction)
            {
                // Leeway label
                screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_VALUE_WARNING_WEEKS].top };
                ft = Formatter();
                ft.Add<rct_string_id>(STR_LEEWAY_WEEKS);
                DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

                // value
                screenCoords = w->windowPos
                    + ScreenCoordsXY{ w->widgets[WIDX_VALUE_WARNING_WEEKS].left + 1, w->widgets[WIDX_VALUE_WARNING_WEEKS].top };
                ft = Formatter();
                ft.Add<rct_string_id>(STR_FORMAT_INTEGER);
                ft.Add<uint16_t>(goal->GetWarningWeeksPeriod());
                DrawTextEllipsised(
                    dpi, screenCoords, w->widgets[WIDX_VALUE_WARNING_WEEKS].width(), STR_WINDOW_COLOUR_2_STRINGID, ft);
            }
            [[fallthrough]];
        case GoalID::ParkSizeGoal:
            // sign label
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_VALUE_SIGN].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_ARITHMETIC_SIGN);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

            // sign value
            screenCoords = w->windowPos
                + ScreenCoordsXY{ w->widgets[WIDX_VALUE_SIGN].left + 1, w->widgets[WIDX_VALUE_SIGN].top };
            ft = Formatter();
            if (goal->GetSign() == Sign::BiggerThan)
                ft.Add<rct_string_id>(STR_AT_LEAST);
            else
                ft.Add<rct_string_id>(STR_AT_MOST);
            DrawTextEllipsised(dpi, screenCoords, w->widgets[WIDX_VALUE_SIGN].width(), STR_WINDOW_COLOUR_2_STRINGID, ft);
            break;
        case GoalID::NumPositiveAwardsGoal:
            // Number of postive awards label
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_NUM_AWARDS].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_NUMBER_OF_AWARDS);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

            // value
            screenCoords = w->windowPos
                + ScreenCoordsXY{ w->widgets[WIDX_NUM_AWARDS].left + 1, w->widgets[WIDX_NUM_AWARDS].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_FORMAT_INTEGER);
            ft.Add<uint16_t>(std::static_pointer_cast<ObjectiveNumPositiveAwardsGoal>(goal)->GetNumAwards());
            DrawTextEllipsised(dpi, screenCoords, w->widgets[WIDX_NUM_AWARDS].width(), STR_WINDOW_COLOUR_2_STRINGID, ft);
            break;
        case GoalID::SpecificTrackedRideGoal:
            DrawStat<uint16_t>(
                w, dpi, WIDX_BUILD_TRACK_INTENSITY_MIN, WIDX_BUILD_TRACK_INTENSITY_MAX, STR_MIN_INTENSITY_RATING,
                STR_MAX_INTENSITY_RATING,
                std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinIntensityGoal(),
                std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxIntensityGoal(), STR_EX_IN_NAU_RATING,
                true);
            DrawStat<uint16_t>(
                w, dpi, WIDX_BUILD_TRACK_NAUSEA_MIN, WIDX_BUILD_TRACK_NAUSEA_MAX, STR_MIN_NAUSEA_RATING, STR_MAX_NAUSEA_RATING,
                std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinNauseaGoal(),
                std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxNauseaGoal(), STR_EX_IN_NAU_RATING,
                true);
            DrawStat<uint16_t>(
                w, dpi, WIDX_BUILD_TRACK_EXCITEMENT_MIN, WIDX_BUILD_TRACK_EXCITEMENT_MAX, STR_MIN_EXCITEMENT_RATING,
                STR_MAX_EXCITEMENT_RATING,
                std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinExcitementGoal(),
                std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxExcitementGoal(), STR_EX_IN_NAU_RATING,
                true);
            DrawStat<uint16_t>(
                w, dpi, WIDX_BUILD_TRACK_LENGTH_MIN, WIDX_BUILD_TRACK_LENGTH_MAX, STR_MIN_TRACK_LENGTH, STR_MAX_TRACK_LENGTH,
                std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinLengthGoal(),
                std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxLengthGoal(), STR_RIDE_LENGTH_ENTRY,
                true);
            if (GetRideTypeDescriptor(std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetBaseRideType())
                    .HasFlag(RIDE_TYPE_FLAG_HAS_DROPS))
            {
                DrawStat<uint16_t>(
                    w, dpi, WIDX_BUILD_TRACK_DROPS_MIN, WIDX_BUILD_TRACK_DROPS_MAX, STR_SELECT_MIN_DROPS,
                    STR_SELECT_MAX_DROPS, 
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinDropCountGoal(),
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxDropCountGoal(), STR_FORMAT_INTEGER,
                    true);
                DrawStat<uint16_t>(
                    w, dpi, WIDX_BUILD_TRACK_INVERSIONS_MIN, WIDX_BUILD_TRACK_INVERSIONS_MAX, STR_SELECT_MIN_INVERSIONS,
                    STR_SELECT_MAX_INVERSIONS,
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinNumInversions(),
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxNumInversions(), STR_FORMAT_INTEGER,
                    true);
                DrawStat<uint16_t>(
                    w, dpi, WIDX_BUILD_TRACK_HEIGHT_MIN, WIDX_BUILD_TRACK_HEIGHT_MAX, STR_SELECT_MIN_HI_DROP,
                    STR_SELECT_MAX_HI_DROP,
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinDropHeightGoal(),
                    std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxDropHeightGoal(),
                    STR_RIDE_LENGTH_ENTRY, true);
            }
            DrawStat<uint16_t>(
                w, dpi, WIDX_BUILD_TRACK_SPEED_MIN, WIDX_BUILD_TRACK_SPEED_MAX, STR_SELECT_MIN_SPEED, STR_SELECT_MAX_SPEED,
                std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMinSpeedGoal(),
                std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetMaxSpeedGoal(), STR_RIDE_VELOCITY, true);

            // Select ride type label
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_BUILD_TRACK_ADD_PIECE].top - 15 };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_REQUIRED_TRACK_PIECES);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

            [[fallthrough]];
        case GoalID::SpecificRideGoal:
            if ((goalID == GoalID::SpecificRideGoal
                 && !std::static_pointer_cast<ObjectiveSpecificRideGoal>(goal)->GetBuildBestGoal())
                || goalID != GoalID::SpecificRideGoal)
            {
                // Must be unique type label
                screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_BUILD_RIDE_UNIQUE].top };
                if (goalID == GoalID::SpecificTrackedRideGoal)
                    screenCoords += ScreenCoordsXY{ 297, 0 };

                ft = Formatter();
                ft.Add<rct_string_id>(STR_MUST_BE_UNIQUE_RIDE);
                DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            }
            if (goalID == GoalID::SpecificRideGoal)
            {
                // Build best type label
                screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_BUILD_RIDE_BEST].top };
                ft = Formatter();
                ft.Add<rct_string_id>(STR_COMPETION_GOAL);
                DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            }
            [[fallthrough]];
        case GoalID::ResearchCertainRide:
        {
            // Select ride type label
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_BUILD_RIDE].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_SELECT_RIDE_TYPE);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

            // value
            rct_string_id rideTypeString = STR_NONE;

            uint16_t baseRideType = std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetBaseRideType();
            uint16_t entryIndex = std::static_pointer_cast<ObjectiveResearchRide>(goal)->GetEntryIndex();

            rct_ride_entry* rideEntry = get_ride_entry(entryIndex);
            if (!GetRideTypeDescriptor(baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
                rideTypeString = get_ride_naming(baseRideType, get_ride_entry(entryIndex)).Name;
            else if (rideEntry != nullptr)
                rideTypeString = rideEntry->naming.Name;
            else
                rideTypeString = GetRideTypeDescriptor(baseRideType).Naming.Name;

            ft = Formatter();
            ft.Add<rct_string_id>(rideTypeString);
            screenCoords = w->windowPos
                + ScreenCoordsXY{ w->widgets[WIDX_BUILD_RIDE].left + 1, w->widgets[WIDX_BUILD_RIDE].top };
            DrawTextEllipsised(dpi, screenCoords, w->widgets[WIDX_BUILD_RIDE].width(), STR_WINDOW_COLOUR_2_STRINGID, ft);
        }
        break;
        case GoalID::NoDuplicateRidesGoal:
            // sign label
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_AWARD].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_SELECT_CATEGORY);
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

            // value
            screenCoords = w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_AWARD].left + 1, w->widgets[WIDX_AWARD].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_STRINGID);
            switch (std::static_pointer_cast<ObjectiveNoDuplicateRidesGoal>(goal)->GetCategory())
            {
                case RIDE_CATEGORY_TRANSPORT:
                case RIDE_CATEGORY_GENTLE:
                case RIDE_CATEGORY_ROLLERCOASTER:
                case RIDE_CATEGORY_THRILL:
                case RIDE_CATEGORY_WATER:
                case RIDE_CATEGORY_SHOP:
                    ft.Add<rct_string_id>(
                        STR_RESEARCH_CATEGORY_TRANSPORT
                        + std::static_pointer_cast<ObjectiveNoDuplicateRidesGoal>(goal)->GetCategory());
                    break;
                case 6: // all but shops
                    ft.Add<rct_string_id>(STR_ALL_RIDES);
                    break;
                case 7: // all
                    ft.Add<rct_string_id>(STR_ALL_RIDES_AND_STALLS);
                    break;
            }
            DrawTextEllipsised(dpi, screenCoords, w->widgets[WIDX_AWARD].width(), STR_WINDOW_COLOUR_2_STRINGID, ft);
        default:
            break;
    }

    // This bit for the value in value-sign goals
    switch (goalID)
    {
        case GoalID::ProfitGoal:
        case GoalID::FoodProfitGoal:
        case GoalID::MerchandiseProfitGoal:
        case GoalID::ParkEntryProfitGoal:
        case GoalID::RideTicketProfitGoal:
        case GoalID::StallProfitGoal:
            // income
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_VALUE_SET_AMOUNT].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_PROFIT_STRING);
            ft.Add<money32>(std::static_pointer_cast<ObjectiveProfitGoal>(goal)->GetProfitGoal());
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            break;
        case GoalID::GuestNumGoal:
            // Guestnum
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_VALUE_SET_AMOUNT].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_GUEST_NUM_STRING);
            ft.Add<uint32_t>(std::static_pointer_cast<ObjectiveGuestNumGoal>(goal)->GetGuestNumGoal());
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            break;
        case GoalID::ParkRatingGoal:
            // Park rating
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_VALUE_SET_AMOUNT].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_PARK_RATING_STRING);
            ft.Add<uint32_t>(std::static_pointer_cast<ObjectiveParkRatingGoal>(goal)->GetParkRatingGoal());
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            break;
        case GoalID::ParkSizeGoal:
            // Park Size
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_VALUE_SET_AMOUNT].top };
            ft = Formatter();
            if (gConfigGeneral.measurement_format == MeasurementFormat::Imperial)
            {
                ft.Add<rct_string_id>(STR_PARK_SIZE_STRING_IMPERIAL);
                ft.Add<uint32_t>(squaredmetres_to_squaredfeet(
                    std::static_pointer_cast<ObjectiveParkSizeGoal>(goal)->GetParkSizeGoal() * 10));
            }
            else
            {
                ft.Add<rct_string_id>(STR_PARK_SIZE_STRING_METRIC);
                ft.Add<uint32_t>(std::static_pointer_cast<ObjectiveParkSizeGoal>(goal)->GetParkSizeGoal() * 10);
            }
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            break;
        case GoalID::ParkValueGoal:
            // Park Value
            screenCoords = w->windowPos + ScreenCoordsXY{ 8, w->widgets[WIDX_VALUE_SET_AMOUNT].top };
            ft = Formatter();
            ft.Add<rct_string_id>(STR_PARK_VALUE_STRING);
            ft.Add<money32>(std::static_pointer_cast<ObjectiveParkValueGoal>(goal)->GetParkValueGoal());
            DrawTextBasic(dpi, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);
            break;
    }
}

static void WindowEditorObjectiveOptionsAddObjectiveScrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    // Draw background
    uint8_t paletteIndex = ColourMapA[w->colours[1]].mid_light;
    gfx_clear(dpi, paletteIndex);

    int32_t itemY = 0; //-SCROLLABLE_ROW_HEIGHT;
    int16_t boxWidth = w->widgets[WIDX_BUILD_TRACK_SCROLL].width();

    auto pieces = std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->GetTrackPiecesRequired();

    size_t i = 0;
    auto ft = Formatter();
    for (auto piece : *pieces)
    {
        ft = Formatter();
        if (deleteTrackPiecesMode)
        {
            ft.Add<rct_string_id>(RideConfigurationStringIds[piece]);
            DrawTextEllipsised(dpi, { 1, itemY }, boxWidth, STR_RED_STRINGID, ft, { COLOUR_BRIGHT_RED });

            if (i == _highlightedIndex)
                gfx_filter_rect(
                    dpi, { 0, itemY, boxWidth, itemY + (SCROLLABLE_ROW_HEIGHT - 1) }, FilterPaletteID::PaletteDarken1);
        }
        else
        {
            ft.Add<rct_string_id>(RideConfigurationStringIds[piece]);
            DrawTextEllipsised(dpi, { 1, itemY }, boxWidth, STR_WINDOW_COLOUR_2_STRINGID, ft);
        }

        itemY += SCROLLABLE_ROW_HEIGHT;
        i++;
    }
}
static void WindowEditorObjectiveOptionsAddObjectiveScrollgetheight(
    rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *height = w->no_list_items * LIST_ROW_HEIGHT;
}

static uint32_t WindowEditorObjectiveOptionsGetItemFromScrollY(int32_t scrollIndex, int32_t y)
{
    return y / SCROLLABLE_ROW_HEIGHT;
}

static void WindowEditorObjectiveOptionsAddObjectivecrollmousedown(
    rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    uint32_t index = WindowEditorObjectiveOptionsGetItemFromScrollY(scrollIndex, screenCoords.y);

    w->Invalidate();
    if (deleteTrackPiecesMode)
    {
        std::static_pointer_cast<ObjectiveSpecificTrackedRideGoal>(goal)->RemoveTrackPieceRequired(index);
    }
}
static void WindowEditorObjectiveOptionsAddObjectiveScrollmouseover(
    rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    if (deleteTrackPiecesMode)
    {
        auto i = static_cast<size_t>(screenCoords.y / SCROLLABLE_ROW_HEIGHT);
        if (i != _highlightedIndex)
        {
            _highlightedIndex = static_cast<size_t>(i);
            w->Invalidate();
        }
    }
    else
    {
        _highlightedIndex = -1;
    }
}
