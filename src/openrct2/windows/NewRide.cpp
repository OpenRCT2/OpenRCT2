#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../config/Config.h"
#include "../network/network.h"
#include "../OpenRCT2.h"
#include "../ride/RideGroupManager.h"
#include "../ride/TrackDesignRepository.h"
#include "../core/Util.hpp"
#include "../core/Math.hpp"

extern "C"
{
    #include "../audio/audio.h"
    #include "../game.h"
    #include "../interface/widget.h"
    #include "../localisation/localisation.h"
    #include "../management/news_item.h"
    #include "../rct1.h"
    #include "../ride/ride_data.h"
    #include "../ride/track_data.h"
    #include "../sprites.h"
    #include "../util/util.h"
}

static uint8 _windowNewRideCurrentTab;
static ride_list_item _windowNewRideHighlightedItem[6];
static ride_list_item _windowNewRideListItems[384];

#pragma region Ride type view order

/**
 * The order of ride types shown in the new ride window so that the order stays consistent across games and rides of the same
 * type are kept together.
 */
const char RideTypeViewOrder[] = {
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
    RIDE_TYPE_WOODEN_ROLLER_COASTER,
    RIDE_TYPE_WOODEN_WILD_MOUSE,
    RIDE_TYPE_WILD_MOUSE,
    RIDE_TYPE_INVERTED_HAIRPIN_COASTER,
    RIDE_TYPE_JUNIOR_ROLLER_COASTER,
    RIDE_TYPE_MINI_ROLLER_COASTER,
    RIDE_TYPE_SPIRAL_ROLLER_COASTER,
    RIDE_TYPE_MINE_TRAIN_COASTER,
    RIDE_TYPE_LOOPING_ROLLER_COASTER,
    RIDE_TYPE_STAND_UP_ROLLER_COASTER,
    RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
    RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER,
    RIDE_TYPE_TWISTER_ROLLER_COASTER,
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
    RIDE_TYPE_MINI_HELICOPTERS,
    RIDE_TYPE_SPIRAL_SLIDE,
    RIDE_TYPE_DODGEMS,
    RIDE_TYPE_SPACE_RINGS,
    RIDE_TYPE_CIRCUS_SHOW,
    RIDE_TYPE_GHOST_TRAIN,
    RIDE_TYPE_FLYING_SAUCERS,

    // Thrill rides
    RIDE_TYPE_TWIST,
    RIDE_TYPE_MAGIC_CARPET,
    RIDE_TYPE_LAUNCHED_FREEFALL,
    RIDE_TYPE_PIRATE_SHIP,
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
    RIDE_TYPE_BOAT_RIDE,
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
    RIDE_TYPE_TOILETS
};

#pragma endregion

enum {
    WINDOW_NEW_RIDE_PAGE_TRANSPORT,
    WINDOW_NEW_RIDE_PAGE_GENTLE,
    WINDOW_NEW_RIDE_PAGE_ROLLER_COASTER,
    WINDOW_NEW_RIDE_PAGE_THRILL,
    WINDOW_NEW_RIDE_PAGE_WATER,
    WINDOW_NEW_RIDE_PAGE_SHOP,
    WINDOW_NEW_RIDE_PAGE_RESEARCH,
    WINDOW_NEW_RIDE_PAGE_COUNT
};

#pragma region Widgets

enum {
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
    WIDX_RESEARCH_FUNDING_BUTTON
};

static rct_widget window_new_ride_widgets[] = {
    { WWT_FRAME,            0,  0,      600,    0,      369,    0xFFFFFFFF,                             STR_NONE                            },
    { WWT_CAPTION,          0,  1,      599,    1,      14,     0xFFFFFFFF,                             STR_WINDOW_TITLE_TIP                },
    { WWT_CLOSEBOX,         0,  588,    598,    2,      13,     STR_CLOSE_X,                            STR_CLOSE_WINDOW_TIP                },
    { WWT_RESIZE,           1,  0,      600,    43,     369,    0xFFFFFFFF,                             STR_NONE                            },
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_TRANSPORT_RIDES_TIP             },
    { WWT_TAB,              1,  34,     64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_GENTLE_RIDES_TIP                },
    { WWT_TAB,              1,  65,     95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_ROLLER_COASTERS_TIP             },
    { WWT_TAB,              1,  96,     126,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_THRILL_RIDES_TIP                },
    { WWT_TAB,              1,  127,    157,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_WATER_RIDES_TIP                 },
    { WWT_TAB,              1,  158,    188,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_SHOPS_STALLS_TIP                },
    { WWT_TAB,              1,  189,    219,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,                   STR_RESEARCH_AND_DEVELOPMENT_TIP    },
    { WWT_SCROLL,           1,  3,      597,    46,     317,    SCROLL_VERTICAL,                                    STR_NONE                            },
    { WWT_GROUPBOX,         2,  3,      292,    47,     116,    STR_CURRENTLY_IN_DEVELOPMENT,           STR_NONE                            },
    { WWT_GROUPBOX,         2,  3,      292,    124,    188,    STR_LAST_DEVELOPMENT,                   STR_NONE                            },
    { WWT_FLATBTN,          2,  265,    288,    161,    184,    0xFFFFFFFF,                             STR_RESEARCH_SHOW_DETAILS_TIP       },
    { WWT_FLATBTN,          2,  265,    288,    68,     91,     SPR_FINANCE,                            STR_FINANCES_RESEARCH_TIP           },
    { WIDGETS_END },
};

#pragma endregion

#pragma region Events

static void window_new_ride_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_new_ride_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_new_ride_update(rct_window *w);
static void window_new_ride_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_new_ride_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_new_ride_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_new_ride_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_new_ride_invalidate(rct_window *w);
static void window_new_ride_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_new_ride_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

// 0x0098E354
static rct_window_event_list window_new_ride_events = {
    nullptr,
    window_new_ride_mouseup,
    nullptr,
    window_new_ride_mousedown,
    nullptr,
    nullptr,
    window_new_ride_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_new_ride_scrollgetsize,
    window_new_ride_scrollmousedown,
    nullptr,
    window_new_ride_scrollmouseover,
    nullptr,
    nullptr,
    nullptr,
    window_new_ride_tooltip,
    nullptr,
    nullptr,
    window_new_ride_invalidate,
    window_new_ride_paint,
    window_new_ride_scrollpaint
};

#pragma endregion

static const rct_string_id window_new_ride_titles[WINDOW_NEW_RIDE_PAGE_COUNT] = {
    STR_NEW_TRANSPORT_RIDES,
    STR_NEW_GENTLE_RIDES,
    STR_NEW_ROLLER_COASTERS,
    STR_NEW_THRILL_RIDES,
    STR_NEW_WATER_RIDES,
    STR_NEW_SHOPS_STALLS,
    STR_RESEARCH_AND_DEVELOPMENT,
};

const sint32 window_new_ride_tab_animation_loops[] = { 20, 32, 10, 72, 24, 28, 16 };
const sint32 window_new_ride_tab_animation_divisor[] = { 4, 8, 2, 4, 4, 4, 2 };

static void window_new_ride_set_page(rct_window *w, sint32 page);
static void window_new_ride_refresh_widget_sizing(rct_window *w);
static ride_list_item window_new_ride_scroll_get_ride_list_item_at(rct_window *w, sint32 x, sint32 y);
static void window_new_ride_paint_ride_information(rct_window *w, rct_drawpixelinfo *dpi, ride_list_item item, sint32 x, sint32 y, sint32 width);
static void window_new_ride_select(rct_window *w);
static ride_list_item * window_new_ride_iterate_over_ride_group(uint8 rideType, uint8 rideGroupIndex, ride_list_item * nextListItem);

static ride_list_item _lastTrackDesignCountRideType;
static sint32 _lastTrackDesignCount;
static bool _trackSelectionByType;

/**
*
*  rct2: 0x006ACA58
*/
void window_new_ride_init_vars()
{
    // If we are in the track designer, default to the Roller Coaster tab
    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) {
        _windowNewRideCurrentTab = WINDOW_NEW_RIDE_PAGE_ROLLER_COASTER;
    } else {
        _windowNewRideCurrentTab = WINDOW_NEW_RIDE_PAGE_TRANSPORT;
    }

    for (sint16 i = 0; i < 6; i++) {
        /*
        Reset what is highlighted in each tab.
        Each 16bit number represents the item in its respective tab.
        */
        _windowNewRideHighlightedItem[i] = { 255, 255 };
    }
}

/**
 *  rct2: 0x006B6F3E
 *
 * Note: When the user has selection by track type enabled, the categories are determined by the track type, not those in the rct_ride_entry.
 */
static void window_new_ride_populate_list()
{
    uint8 currentCategory = _windowNewRideCurrentTab;
    ride_list_item *nextListItem = _windowNewRideListItems;

    // For each ride type in the view order list
    for (sint32 i = 0; i < (sint32)Util::CountOf(RideTypeViewOrder); i++) {
        uint8 rideType = RideTypeViewOrder[i];
        if (rideType == RIDE_TYPE_NULL)
            continue;

        if(gConfigInterface.select_by_track_type) {
            if(gRideCategories[rideType]!=currentCategory)
                continue;
        }

        if (ride_type_is_invented(rideType) || gCheatsIgnoreResearchStatus) {

            if (!ride_type_has_ride_groups(rideType)) {
                nextListItem = window_new_ride_iterate_over_ride_group(rideType, 0, nextListItem);
            }
            else {
                for (uint8 j = 0; j < MAX_RIDE_GROUPS_PER_RIDE_TYPE; j++) {
                    nextListItem = window_new_ride_iterate_over_ride_group(rideType, j, nextListItem);
                }
            }
        }
    }

    nextListItem->type = RIDE_TYPE_NULL;
    nextListItem->entry_index = RIDE_ENTRY_INDEX_NULL;
    _trackSelectionByType = gConfigInterface.select_by_track_type;
}

static ride_list_item * window_new_ride_iterate_over_ride_group(uint8 rideType, uint8 rideGroupIndex, ride_list_item * nextListItem)
{
    uint8 currentCategory = _windowNewRideCurrentTab;

    bool buttonForRideTypeCreated = false;
    bool allowDrawingOverLastButton = false;
    uint8 *rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(rideType);

    char preferredVehicleName[9];
    safe_strcpy(preferredVehicleName, "        ", sizeof(preferredVehicleName));

    // For each ride entry for this ride type
    while (*rideEntryIndexPtr != RIDE_ENTRY_INDEX_NULL) {
        uint8 rideEntryIndex = *rideEntryIndexPtr++;
        char rideEntryName[9];
        memcpy(rideEntryName,object_entry_groups[OBJECT_TYPE_RIDE].entries[rideEntryIndex].name,8);
        rideEntryName[8]=0;

        // Skip if vehicle type is not invented yet
        if (!ride_entry_is_invented(rideEntryIndex) && !gCheatsIgnoreResearchStatus)
            continue;

        // Ride entries
        rct_ride_entry *rideEntry = get_ride_entry(rideEntryIndex);

        // Check if ride is in this category
        if (!gConfigInterface.select_by_track_type && !ride_entry_has_category(rideEntry, currentCategory))
            continue;

        if (ride_type_has_ride_groups(rideType))
        {
            const ride_group * rideEntryRideGroup = get_ride_group(rideType, rideEntry);
            const ride_group * rideGroup = ride_group_find(rideType, rideGroupIndex);

			if (!ride_groups_are_equal(rideEntryRideGroup, rideGroup))
                continue;
        }

        // Skip if the vehicle isn't the preferred vehicle for this generic track type
        if (gConfigInterface.select_by_track_type && (!(rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE) || rideTypeShouldLoseSeparateFlag(rideEntry))) {
            if (strcmp(preferredVehicleName, "        \0") == 0) {
                safe_strcpy(preferredVehicleName, rideEntryName, sizeof(preferredVehicleName));
                preferredVehicleName[8] = 0;
            } else {
                if (vehicle_preference_compare(rideType, preferredVehicleName, rideEntryName) == 1) {
                    safe_strcpy(preferredVehicleName, rideEntryName, sizeof(preferredVehicleName));
                    preferredVehicleName[8] = 0;
                } else {
                    continue;
                }
            }
        }

        // Determines how and where to draw a button for this ride type/vehicle.
        if ((rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE) && !rideTypeShouldLoseSeparateFlag(rideEntry)) { // Separate, draw apart
            allowDrawingOverLastButton = false;
            nextListItem->type = rideType;
            nextListItem->entry_index = rideEntryIndex;
            nextListItem++;
        } else if (!buttonForRideTypeCreated) { // Non-separate, draw-apart
            // Draw apart
            buttonForRideTypeCreated = true;
            allowDrawingOverLastButton = true;
            nextListItem->type = rideType;
            nextListItem->entry_index = rideEntryIndex;
            nextListItem++;
        } else if (allowDrawingOverLastButton) { // Non-separate, draw over previous
            if (rideType == rideEntry->ride_type[0]) {
                nextListItem--;
                nextListItem->type = rideType;
                nextListItem->entry_index = rideEntryIndex;
                nextListItem++;
            }
        }
    }

    return nextListItem;
}

/**
 *
 *  rct2: 0x006B7220
 */
static void window_new_ride_scroll_to_focused_ride(rct_window *w)
{
    sint32 scrollWidth = 0;
    sint32 scrollHeight = 0;
    window_get_scroll_size(w, 0, &scrollWidth, &scrollHeight);

    // Find row index of the focused ride type
    rct_widget *listWidget = &window_new_ride_widgets[WIDX_RIDE_LIST];
    assert(_windowNewRideCurrentTab < Util::CountOf(_windowNewRideHighlightedItem));
    sint32 focusRideType = _windowNewRideHighlightedItem[_windowNewRideCurrentTab].ride_type_and_entry;
    sint32 count = 0, row = 0;
    ride_list_item *listItem = _windowNewRideListItems;
    while (listItem->type != RIDE_TYPE_NULL || listItem->entry_index != 255) {
        if (listItem->type == focusRideType) {
            row = count / 5;
            break;
        }

        count++;
        listItem++;
    };

    // Update the Y scroll position
    sint32 listWidgetHeight = listWidget->bottom - listWidget->top - 1;
    scrollHeight = Math::Max(0, scrollHeight - listWidgetHeight);
    w->scrolls[0].v_top = Math::Min(row * 116, scrollHeight);
    widget_scroll_update_thumbs(w, WIDX_RIDE_LIST);
}

/**
 *
 *  rct2: 0x006B3CFF
 */
rct_window *window_new_ride_open()
{
    rct_window *w;

    w = window_bring_to_front_by_class(WC_CONSTRUCT_RIDE);
    if (w != nullptr)
        return w;

    // Not sure what these windows are
    window_close_by_class(WC_TRACK_DESIGN_LIST);
    window_close_by_class(WC_TRACK_DESIGN_PLACE);

    w = window_create_auto_pos(601, 370, &window_new_ride_events, WC_CONSTRUCT_RIDE, WF_10);
    w->widgets = window_new_ride_widgets;
    w->enabled_widgets =
        (1 << WIDX_CLOSE) |
        (1 << WIDX_TAB_1) |
        (1 << WIDX_TAB_2) |
        (1 << WIDX_TAB_3) |
        (1 << WIDX_TAB_4) |
        (1 << WIDX_TAB_5) |
        (1 << WIDX_TAB_6) |
        (1 << WIDX_TAB_7) |
        (1 << WIDX_LAST_DEVELOPMENT_BUTTON) |
        (1 << WIDX_RESEARCH_FUNDING_BUTTON);
    window_new_ride_populate_list();
    window_init_scroll_widgets(w);

    w->frame_no = 0;
    w->new_ride.selected_ride_id = -1;
    _lastTrackDesignCountRideType.type = RIDE_TYPE_NULL;
    _lastTrackDesignCountRideType.entry_index = RIDE_ENTRY_INDEX_NULL;
    w->new_ride.highlighted_ride_id = _windowNewRideHighlightedItem[_windowNewRideCurrentTab].ride_type_and_entry;
    if (w->new_ride.highlighted_ride_id == -1)
        w->new_ride.highlighted_ride_id = _windowNewRideListItems[0].ride_type_and_entry;

    w->width = 1;
    window_new_ride_refresh_widget_sizing(w);

    if (_windowNewRideCurrentTab != WINDOW_NEW_RIDE_PAGE_RESEARCH) {
        window_new_ride_scroll_to_focused_ride(w);
    }

    return w;
}

rct_window *window_new_ride_open_research()
{
    rct_window *w;

    w = window_new_ride_open();
    window_new_ride_set_page(w, WINDOW_NEW_RIDE_PAGE_RESEARCH);
    return w;
}

/**
 *
 *  rct2: 0x006B3EBA
 */
void window_new_ride_focus(ride_list_item rideItem)
{
    rct_window *w;
    rct_ride_entry *rideEntry;
    bool entryFound = false;
    // Find the first non-null ride type index.

    w = window_find_by_class(WC_CONSTRUCT_RIDE);
    if (w == nullptr)
        return;

    rideEntry = get_ride_entry(rideItem.entry_index);
    uint8 rideTypeIndex = ride_entry_get_first_non_null_ride_type(rideEntry);

    if(!gConfigInterface.select_by_track_type) {
        window_new_ride_set_page(w, rideEntry->category[0]);
    }
    else {
        window_new_ride_set_page(w, gRideCategories[rideTypeIndex]);
    }

    for (ride_list_item *listItem = _windowNewRideListItems; listItem->type != RIDE_TYPE_NULL; listItem++) {
        if (listItem->type == rideItem.type && listItem->entry_index == rideItem.entry_index) {
            _windowNewRideHighlightedItem[0] = rideItem;
            w->new_ride.highlighted_ride_id = rideItem.ride_type_and_entry;
            window_new_ride_scroll_to_focused_ride(w);
            entryFound = true;
            break;
        }
    }

    // If this entry was not found it was most likely hidden due to it not being the preferential type.
    // In this case, select the first entry that belongs to the same ride group.
    if (!entryFound && gConfigInterface.select_by_track_type)
    {
        const ride_group * rideGroup = get_ride_group(rideTypeIndex, rideEntry);

        for (ride_list_item *listItem = _windowNewRideListItems; listItem->type != RIDE_TYPE_NULL; listItem++) {
            if (listItem->type == rideItem.type) {
                const ride_group * irg = get_ride_group(rideTypeIndex, rideEntry);

                if (!ride_type_has_ride_groups(rideTypeIndex) || ride_groups_are_equal(rideGroup, irg)) {
                    _windowNewRideHighlightedItem[0] = rideItem;
                    w->new_ride.highlighted_ride_id = rideItem.ride_type_and_entry;
                    window_new_ride_scroll_to_focused_ride(w);
                    break;
                }
            }
        }
    }
}

static void window_new_ride_set_page(rct_window *w, sint32 page)
{
    _windowNewRideCurrentTab = page;
    w->frame_no = 0;
    w->new_ride.highlighted_ride_id = -1;
    w->new_ride.selected_ride_countdown = -1;
    window_new_ride_populate_list();
    if (page < WINDOW_NEW_RIDE_PAGE_RESEARCH) {
        w->new_ride.highlighted_ride_id = _windowNewRideHighlightedItem[page].ride_type_and_entry;
        if (w->new_ride.highlighted_ride_id == -1)
            w->new_ride.highlighted_ride_id = _windowNewRideListItems[0].ride_type_and_entry;
    }

    window_new_ride_refresh_widget_sizing(w);
    window_invalidate(w);

    if (page < WINDOW_NEW_RIDE_PAGE_RESEARCH) {
        window_new_ride_scroll_to_focused_ride(w);
    }
}

/**
 *
 *  rct2: 0x006B3DF1
 */
static void window_new_ride_refresh_widget_sizing(rct_window *w)
{
    sint32 width, height;

    // Show or hide unrelated widgets
    if (_windowNewRideCurrentTab != WINDOW_NEW_RIDE_PAGE_RESEARCH) {
        window_new_ride_widgets[WIDX_RIDE_LIST].type = WWT_SCROLL;
        window_new_ride_widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].type = WWT_EMPTY;
        window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_GROUP].type = WWT_EMPTY;
        window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_EMPTY;
        window_new_ride_widgets[WIDX_RESEARCH_FUNDING_BUTTON].type = WWT_EMPTY;

        width = 601;
        height = 370;
    } else {
        window_new_ride_widgets[WIDX_RIDE_LIST].type = WWT_EMPTY;
        window_new_ride_widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].type = WWT_GROUPBOX;
        window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_GROUP].type = WWT_GROUPBOX;
        window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_FLATBTN;
        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
            window_new_ride_widgets[WIDX_RESEARCH_FUNDING_BUTTON].type = WWT_FLATBTN;

        width = 300;
        height = 196;
    }

    // Handle new window size
    if (w->width != width || w->height != height) {
        window_invalidate(w);

        // Resize widgets to new window size
        window_new_ride_widgets[WIDX_BACKGROUND].right = width - 1;
        window_new_ride_widgets[WIDX_BACKGROUND].bottom = height - 1;
        window_new_ride_widgets[WIDX_PAGE_BACKGROUND].right = width - 1;
        window_new_ride_widgets[WIDX_PAGE_BACKGROUND].bottom = height - 1;
        window_new_ride_widgets[WIDX_TITLE].right = width - 2;
        window_new_ride_widgets[WIDX_CLOSE].left = width - 13;
        window_new_ride_widgets[WIDX_CLOSE].right = width - 3;

        w->width = width;
        w->height = height;
        window_invalidate(w);
    }

    window_init_scroll_widgets(w);
}

static void window_new_ride_set_pressed_tab(rct_window *w)
{
    sint32 i;
    for (i = 0; i < WINDOW_NEW_RIDE_PAGE_COUNT; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + _windowNewRideCurrentTab);
}

const sint32 ThrillRidesTabAnimationSequence[] = {
    5, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0
};

static void window_new_ride_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, sint32 page, sint32 spriteIndex)
{
    rct_widgetindex widgetIndex = WIDX_TAB_1 + page;

    if (w->widgets[widgetIndex].type != WWT_EMPTY && !(w->disabled_widgets & (1LL << widgetIndex))) {
        sint32 frame = 0;
        if (_windowNewRideCurrentTab == page)
            frame = w->frame_no / window_new_ride_tab_animation_divisor[page];

        spriteIndex += page == WINDOW_NEW_RIDE_PAGE_THRILL ?
                ThrillRidesTabAnimationSequence[frame] : frame;

        spriteIndex |= w->colours[1] << 19;

        gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
    }
}

static void window_new_ride_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
    window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_TRANSPORT, IMAGE_TYPE_REMAP | SPR_TAB_RIDES_TRANSPORT_0);
    window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_GENTLE, SPR_TAB_RIDES_GENTLE_0);
    window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_ROLLER_COASTER, IMAGE_TYPE_REMAP | SPR_TAB_RIDES_ROLLER_COASTERS_0);
    window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_THRILL, SPR_TAB_RIDES_THRILL_0);
    window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_WATER, SPR_TAB_RIDES_WATER_0);
    window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_SHOP, SPR_TAB_RIDES_SHOP_0);
    window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_RESEARCH, SPR_TAB_FINANCES_RESEARCH_0);
}

/**
 *
 *  rct2: 0x006B6B38
 */
static void window_new_ride_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_LAST_DEVELOPMENT_BUTTON:
        news_item_open_subject(NEWS_ITEM_RESEARCH, (sint32)gResearchLastItemSubject);
        break;
    case WIDX_RESEARCH_FUNDING_BUTTON:
        window_finances_research_open();
        break;
    }
}

/**
 *
 *  rct2: 0x006B6B4F
 */
static void window_new_ride_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget)
{
    if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
        window_new_ride_set_page(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x006B6CE7
 */
static void window_new_ride_update(rct_window *w)
{
    w->frame_no++;
    if (w->frame_no >= window_new_ride_tab_animation_loops[_windowNewRideCurrentTab])
        w->frame_no = 0;

    widget_invalidate(w, WIDX_TAB_1 + _windowNewRideCurrentTab);

    if (w->new_ride.selected_ride_id != -1 && w->new_ride.selected_ride_countdown-- == 0)
        window_new_ride_select(w);

    if (_trackSelectionByType != gConfigInterface.select_by_track_type) {
        window_new_ride_populate_list();
        widget_invalidate(w, WIDX_RIDE_LIST);
    }
}

/**
 *
 *  rct2: 0x006B6BC9
 */
static void window_new_ride_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
    ride_list_item *listItem = _windowNewRideListItems;

    sint32 count = 0;
    while (listItem->type != RIDE_TYPE_NULL || listItem->entry_index != 255) {
        count++;
        listItem++;
    }
    *height = ((count + 4) / 5) * 116;
}

/**
 *
 *  rct2: 0x006B6C89
 */
static void window_new_ride_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    ride_list_item item;

    item = window_new_ride_scroll_get_ride_list_item_at(w, x, y);
    if (item.type == RIDE_TYPE_NULL && item.entry_index == 255)
        return;

    _windowNewRideHighlightedItem[_windowNewRideCurrentTab] = item;
    w->new_ride.selected_ride_id = item.ride_type_and_entry;

    audio_play_sound(SOUND_CLICK_1, 0, w->x + (w->width / 2));
    w->new_ride.selected_ride_countdown = 8;
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006B6C51
 */
static void window_new_ride_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    ride_list_item item;

    if (w->new_ride.selected_ride_id != -1)
        return;

    item = window_new_ride_scroll_get_ride_list_item_at(w, x, y);

    if (w->new_ride.highlighted_ride_id == item.ride_type_and_entry)
        return;

    w->new_ride.highlighted_ride_id = item.ride_type_and_entry;
    _windowNewRideHighlightedItem[_windowNewRideCurrentTab] = item;
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006B6BBF
 */
static void window_new_ride_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
    set_format_arg(0, rct_string_id, STR_LIST);
}

/**
 *
 *  rct2: 0x006B6819
 */
static void window_new_ride_invalidate(rct_window *w)
{
    window_new_ride_set_pressed_tab(w);

    window_new_ride_widgets[WIDX_TITLE].text = window_new_ride_titles[_windowNewRideCurrentTab];
    window_new_ride_widgets[WIDX_TAB_7].type = WWT_TAB;
    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
        window_new_ride_widgets[WIDX_TAB_7].type = WWT_EMPTY;

    if (_windowNewRideCurrentTab == WINDOW_NEW_RIDE_PAGE_RESEARCH) {
        window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_EMPTY;
        uint32 typeId = gResearchLastItemSubject;
        if (typeId != 0xFFFFFFFF) {
            window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_FLATBTN;
            window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].image = typeId >= 0x10000 ? SPR_NEW_RIDE : SPR_NEW_SCENERY;
        }
    }
}

/**
 *
 *  rct2: 0x006B689B
 */
static void window_new_ride_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_new_ride_draw_tab_images(dpi, w);

    if (_windowNewRideCurrentTab != WINDOW_NEW_RIDE_PAGE_RESEARCH) {
        ride_list_item item;
        item.ride_type_and_entry = static_cast<uint16>(w->new_ride.highlighted_ride_id);
        if (item.type != RIDE_TYPE_NULL || item.entry_index != 255)
            window_new_ride_paint_ride_information(w, dpi, item, w->x + 3, w->y + w->height - 52, w->width - 6);
    } else {
        window_research_development_page_paint(w, dpi, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
    }
}

/**
 *
 *  rct2: 0x006B6ABF
 */
static void window_new_ride_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
    if (_windowNewRideCurrentTab == WINDOW_NEW_RIDE_PAGE_RESEARCH)
        return;

    gfx_clear(dpi, ColourMapA[w->colours[1]].mid_light);

    sint32 x = 1;
    sint32 y = 1;
    ride_list_item *listItem = _windowNewRideListItems;
    while (listItem->type != RIDE_TYPE_NULL || listItem->entry_index != 255) {
        rct_ride_entry *rideEntry;
        // Draw flat button rectangle
        sint32 flags = 0;
        if (w->new_ride.selected_ride_id == *((sint16*)listItem))
            flags |= INSET_RECT_FLAG_BORDER_INSET;
        if (w->new_ride.highlighted_ride_id == *((sint16*)listItem) || flags != 0)
            gfx_fill_rect_inset(dpi, x, y, x + 115, y + 115, w->colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT | flags);

        // Draw ride image with feathered border
        rideEntry = get_ride_entry(listItem->entry_index);
        sint32 image_id = rideEntry->images_offset;
        if (listItem->type != rideEntry->ride_type[0]) {
            image_id++;
            if (listItem->type != rideEntry->ride_type[1])
                image_id++;
        }
        gfx_draw_sprite_raw_masked(dpi, x + 2, y + 2, SPR_NEW_RIDE_MASK, image_id);

        // Next position
        x += 116;
        if (x >= 116 * 5 + 1) {
            x = 1;
            y += 116;
        }

        // Next item
        listItem++;
    }
}

/**
 *
 *  rct2: 0x006B6D3C
 */
static ride_list_item window_new_ride_scroll_get_ride_list_item_at(rct_window *w, sint32 x, sint32 y)
{
    ride_list_item result;
    result.type = RIDE_TYPE_NULL;
    result.entry_index = RIDE_ENTRY_INDEX_NULL;

    if (--x < 0 || --y < 0)
        return result;

    sint32 column = x / 116;
    sint32 row = y / 116;
    if (column >= 5)
        return result;

    sint32 index = column + (row * 5);

    ride_list_item *listItem = _windowNewRideListItems;
    while (listItem->type != RIDE_TYPE_NULL || listItem->entry_index != 255) {
        if (index-- == 0)
            return *listItem;
        listItem++;
    }

    return result;
}

static sint32 get_num_track_designs(ride_list_item item)
{
    char entry[9];
    const char *entryPtr = nullptr;
    rct_ride_entry * rideEntry = nullptr;

    if (item.type < 0x80) {
        rideEntry = get_ride_entry(item.entry_index);
        if ((rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE) && !rideTypeShouldLoseSeparateFlag(rideEntry)) {
            get_ride_entry_name(entry, item.entry_index);
            entryPtr = entry;
        }
    }

    if (rideEntry != nullptr && ride_type_has_ride_groups(item.type))
    {
        const ride_group * rideGroup = get_ride_group(item.type, rideEntry);
        return (sint32)track_repository_get_count_for_ride_group(item.type, rideGroup);
    }

    return (sint32)track_repository_get_count_for_ride(item.type, entryPtr);
}

/**
 *
 *  rct2: 0x006B701C
 */
static void window_new_ride_paint_ride_information(rct_window *w, rct_drawpixelinfo *dpi, ride_list_item item, sint32 x, sint32 y, sint32 width)
{
    rct_ride_entry *rideEntry = get_ride_entry(item.entry_index);
    rct_ride_name rideNaming;

    // Ride name and description
    rideNaming = get_ride_naming(item.type , rideEntry);
    set_format_arg(0, rct_string_id, rideNaming.name);
    set_format_arg(2, rct_string_id, rideNaming.description);
    gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, x, y, width, STR_NEW_RIDE_NAME_AND_DESCRIPTION, COLOUR_BLACK);

    // Number of designs available
    if (ride_type_has_flag(item.type, RIDE_TYPE_FLAG_HAS_TRACK)) {
        // Track designs are disabled in multiplayer, so don't say there are any designs available when in multiplayer
        if (network_get_mode() != NETWORK_MODE_NONE) {
            _lastTrackDesignCount = 0;
        }
        else if (item.type != _lastTrackDesignCountRideType.type || item.entry_index != _lastTrackDesignCountRideType.entry_index) {
            _lastTrackDesignCountRideType = item;
            _lastTrackDesignCount = get_num_track_designs(item);
        }

        rct_string_id stringId;
        switch (_lastTrackDesignCount) {
        case 0:
            stringId = STR_CUSTOM_DESIGNED_LAYOUT;
            break;
        case 1:
            stringId = STR_1_DESIGN_AVAILABLE;
            break;
        default:
            stringId = STR_X_DESIGNS_AVAILABLE;
            break;
        }
        gfx_draw_string_left(dpi, stringId, &_lastTrackDesignCount, COLOUR_BLACK, x, y + 39);
    }

    // Price
    if (!(gParkFlags & PARK_FLAGS_NO_MONEY)) {
        // Get price of ride
        sint32 unk2 = RideConstructionDefaultTrackType[item.type];
        money32 price = RideTrackCosts[item.type].track_price;
        if (ride_type_has_flag(item.type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
            price *= FlatRideTrackPricing[unk2];
        } else {
            price *= TrackPricing[unk2];
        }
        price = (price >> 17) * 10 * RideData5[item.type].price;

        //
        rct_string_id stringId = STR_NEW_RIDE_COST;
        if (!ride_type_has_flag(item.type, RIDE_TYPE_FLAG_HAS_NO_TRACK))
            stringId = STR_NEW_RIDE_COST_FROM;

        gfx_draw_string_right(dpi, stringId, &price, COLOUR_BLACK, x + width, y + 39);
    }
}

/**
 *
 *  rct2: 0x006B6B78
 */
static void window_new_ride_select(rct_window *w)
{
    ride_list_item item;
    item.ride_type_and_entry = static_cast<uint16>(w->new_ride.selected_ride_id);
    if (item.type == RIDE_TYPE_NULL)
        return;

    window_close(w);
    window_close_construction_windows();

    bool allowTrackDesigns = true;
#ifndef NETWORK_DISABLE
    // TODO: FIX NETWORK TRACKS
    // Until tracks work with the network this will disable them
    if (network_get_mode() != NETWORK_MODE_NONE) {
        allowTrackDesigns = false;
    }
#endif

    if (allowTrackDesigns && ride_type_has_flag(item.type, RIDE_TYPE_FLAG_HAS_TRACK)) {
        if (_lastTrackDesignCount > 0) {
            window_track_list_open(item);
            return;
        }
    }

    ride_construct_new(item);
}
