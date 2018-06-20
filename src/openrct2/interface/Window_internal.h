/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <vector>
#include "Window.h"

struct rct_research_item;
struct rct_object_entry;

/**
 * Window structure
 * size: 0x4C0
 */
struct rct_window {
    rct_window_event_list* event_handlers;      // 0x000
    rct_viewport* viewport;     // 0x004
    uint64_t enabled_widgets;     // 0x008
    uint64_t disabled_widgets;    // 0x010
    uint64_t pressed_widgets;     // 0x018
    uint64_t hold_down_widgets;   // 0x020
    rct_widget* widgets;        // 0x028
    int16_t x;                   // 0x02C
    int16_t y;                   // 0x02E
    int16_t width;               // 0x030
    int16_t height;              // 0x032
    int16_t min_width;           // 0x034
    int16_t max_width;           // 0x036
    int16_t min_height;          // 0x038
    int16_t max_height;          // 0x03A
    rct_windownumber number;    // 0x03C
    uint16_t flags;               // 0x03E
    rct_scroll scrolls[3];      // 0x040
    uint8_t list_item_positions[1024];// 0x076
    uint16_t no_list_items;           // 0x476 0 for no items
    int16_t pad_478;
    int16_t selected_list_item;      // 0x47A -1 for none selected
    int16_t pad_47C;
    int16_t pad_47E;
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
    int16_t page;                    // 0x48A
    union {
        int16_t picked_peep_old_x;   // 0x48C staff/guest window: peep x gets set to 0x8000 on pickup, this is the old value
        int16_t vehicleIndex;        // 0x48C Ride window: selected car when setting vehicle colours
        int16_t numberOfStaff;       // 0x48C Used in park window.
        int16_t var_48C;
    };
    uint16_t frame_no;                // 0x48E updated every tic for motion in windows sprites
    uint16_t list_information_type;   // 0x490 0 for none, Used as current position of marquee in window_peep
    union {
        int16_t picked_peep_frame;   // 0x492 Animation frame of picked peep in staff window and guest window
        int16_t var_492;
    };
    union {                         // 0x494
        uint32_t highlighted_item;
        uint16_t ride_colour;
        rct_research_item* research_item;
        rct_object_entry* object_entry;
        const scenario_index_entry* highlighted_scenario;
        struct {
            uint16_t var_494;
            uint16_t var_496;
        };
    };
    uint8_t var_498[0x14];
    int16_t selected_tab;            // 0x4AC
    int16_t var_4AE;
    uint16_t viewport_target_sprite;  // 0x4B0 viewport target sprite
    int16_t saved_view_x;            // 0x4B2
    int16_t saved_view_y;            // 0x4B4
    rct_windowclass classification; // 0x4B6
    uint8_t pad_4B7;
    int8_t var_4B8;
    int8_t var_4B9;
    uint8_t colours[6];           // 0x4BA
    uint8_t visibility;           // VISIBILITY_CACHE
    uint16_t viewport_smart_follow_sprite; // Smart following of sprites. Handles setting viewport target sprite etc
};

// rct2: 0x01420078
extern std::vector<std::unique_ptr<rct_window>> g_window_list;
