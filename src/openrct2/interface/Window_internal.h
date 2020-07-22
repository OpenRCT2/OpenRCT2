/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Window.h"

#include <list>
#include <memory>

struct ResearchItem;
struct rct_object_entry;

/**
 * Window structure
 * size: 0x4C0
 */
struct rct_window
{
    rct_window_event_list* event_handlers;
    rct_viewport* viewport;
    uint64_t enabled_widgets;
    uint64_t disabled_widgets;
    uint64_t pressed_widgets;
    uint64_t hold_down_widgets;
    rct_widget* widgets;
    ScreenCoordsXY windowPos;
    int16_t width;
    int16_t height;
    int16_t min_width;
    int16_t max_width;
    int16_t min_height;
    int16_t max_height;
    rct_windownumber number;
    uint16_t flags;
    rct_scroll scrolls[3];
    uint8_t list_item_positions[1024];
    uint16_t no_list_items;     // 0 for no items
    int16_t selected_list_item; // -1 for none selected
    union
    {
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
        void* custom_info;
    };
    int16_t page;
    union
    {
        int16_t picked_peep_old_x; // staff/guest window: peep x gets set to 0x8000 on pickup, this is the old value
        int16_t vehicleIndex;      // Ride window: selected car when setting vehicle colours
        int16_t numberOfStaff;     // Used in park window.
        int16_t SceneryEntry;      // Used in sign window.
        int16_t var_48C;
    };
    uint16_t frame_no;              // updated every tic for motion in windows sprites
    uint16_t list_information_type; // 0 for none, Used as current position of marquee in window_peep
    union
    {
        int16_t picked_peep_frame; // Animation frame of picked peep in staff window and guest window
        int16_t var_492;
    };
    union
    {
        uint32_t highlighted_item;
        uint16_t ride_colour;
        ResearchItem* research_item;
        rct_object_entry* object_entry;
        const scenario_index_entry* highlighted_scenario;
        uint16_t var_496;
    };
    int16_t selected_tab;
    int16_t var_4AE;
    uint16_t viewport_target_sprite;
    ScreenCoordsXY savedViewPos;
    rct_windowclass classification;
    colour_t colours[6];
    uint8_t visibility;
    uint16_t viewport_smart_follow_sprite; // Handles setting viewport target sprite etc

    void SetLocation(const CoordsXYZ& coords);
    void ScrollToViewport();
    void Invalidate();
};

// rct2: 0x01420078
extern std::list<std::shared_ptr<rct_window>> g_window_list;
