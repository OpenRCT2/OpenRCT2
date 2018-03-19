#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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

#ifndef _WINDOW2_H_
#define _WINDOW2_H_

struct rct_research_item;
struct rct_object_entry;

/**
 * Window structure
 * size: 0x4C0
 */
struct rct_window {
    rct_window_event_list* event_handlers;      // 0x000
    rct_viewport* viewport;     // 0x004
    uint64 enabled_widgets;     // 0x008
    uint64 disabled_widgets;    // 0x010
    uint64 pressed_widgets;     // 0x018
    uint64 hold_down_widgets;   // 0x020
    rct_widget* widgets;        // 0x028
    sint16 x;                   // 0x02C
    sint16 y;                   // 0x02E
    sint16 width;               // 0x030
    sint16 height;              // 0x032
    sint16 min_width;           // 0x034
    sint16 max_width;           // 0x036
    sint16 min_height;          // 0x038
    sint16 max_height;          // 0x03A
    rct_windownumber number;    // 0x03C
    uint16 flags;               // 0x03E
    rct_scroll scrolls[3];      // 0x040
    uint8 list_item_positions[1024];// 0x076
    uint16 no_list_items;           // 0x476 0 for no items
    sint16 pad_478;
    sint16 selected_list_item;      // 0x47A -1 for none selected
    sint16 pad_47C;
    sint16 pad_47E;
    union {
        coordinate_focus viewport_focus_coordinates;
        sprite_focus viewport_focus_sprite;
        campaign_variables campaign;
        new_ride_variables new_ride;
        news_variables news;
        map_variables map;
        ride_variables ride;
        scenery_variables scenery;
        track_list_variables track_list;
        error_variables error;
    };
    sint16 page;                    // 0x48A
    union {
        sint16 picked_peep_old_x;   // 0x48C staff/guest window: peep x gets set to 0x8000 on pickup, this is the old value
        sint16 vehicleIndex;        // 0x48C Ride window: selected car when setting vehicle colours
        sint16 numberOfStaff;       // 0x48C Used in park window.
        sint16 var_48C;
    };
    uint16 frame_no;                // 0x48E updated every tic for motion in windows sprites
    uint16 list_information_type;   // 0x490 0 for none, Used as current position of marquee in window_peep
    union {
        sint16 picked_peep_frame;   // 0x492 Animation frame of picked peep in staff window and guest window
        sint16 var_492;
    };
    union {                         // 0x494
        uint32 highlighted_item;
        uint16 ride_colour;
        rct_research_item* research_item;
        rct_object_entry* object_entry;
        const scenario_index_entry* highlighted_scenario;
        struct {
            uint16 var_494;
            uint16 var_496;
        };
    };
    uint8 var_498[0x14];
    sint16 selected_tab;            // 0x4AC
    sint16 var_4AE;
    uint16 viewport_target_sprite;  // 0x4B0 viewport target sprite
    sint16 saved_view_x;            // 0x4B2
    sint16 saved_view_y;            // 0x4B4
    rct_windowclass classification; // 0x4B6
    uint8 pad_4B7;
    sint8 var_4B8;
    sint8 var_4B9;
    uint8 colours[6];           // 0x4BA
    uint8 visibility;           // VISIBILITY_CACHE
    uint16 viewport_smart_follow_sprite; // Smart following of sprites. Handles setting viewport target sprite etc
};

// rct2: 0x01420078
extern rct_window g_window_list[WINDOW_LIMIT_MAX + WINDOW_LIMIT_RESERVED];

#endif
