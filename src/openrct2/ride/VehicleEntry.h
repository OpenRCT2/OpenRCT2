/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../audio/AudioMixer.h"
#include "../common.h"

#include <array>
#include <vector>

#ifdef __TESTPAINT__
#    pragma pack(push, 1)
#endif // __TESTPAINT__
/**
 * Ride type vehicle structure.
 * size: 0x65
 */
struct rct_ride_entry_vehicle
{
    uint16_t rotation_frame_mask;
    uint8_t num_vertical_frames;   // Appears to be unused, except as a temporary variable in RCT2 (not needed for OpenRCT2)
    uint8_t num_horizontal_frames; // Appears to be unused, except as a temporary variable in RCT2 (not needed for OpenRCT2)
    uint32_t spacing;
    uint16_t car_mass;
    int8_t tab_height;
    uint8_t num_seats;
    uint32_t sprite_flags;
    uint8_t sprite_width;
    uint8_t sprite_height_negative;
    uint8_t sprite_height_positive;
    uint8_t animation;
    uint32_t flags;
    uint16_t base_num_frames; // The number of sprites for a flat non-banked track piece.
    uint32_t base_image_id;   // Following image_id's populated during loading
    uint32_t restraint_image_id;
    uint32_t gentle_slope_image_id;
    uint32_t steep_slope_image_id;
    uint32_t vertical_slope_image_id;
    uint32_t diagonal_slope_image_id;
    uint32_t banked_image_id;
    uint32_t inline_twist_image_id;
    uint32_t flat_to_gentle_bank_image_id;
    uint32_t diagonal_to_gentle_slope_bank_image_id;
    uint32_t gentle_slope_to_bank_image_id;
    uint32_t gentle_slope_bank_turn_image_id;
    uint32_t flat_bank_to_gentle_slope_image_id;
    union
    {
        uint32_t curved_lift_hill_image_id;
        uint32_t corkscrew_image_id;
    };
    uint32_t no_vehicle_images;
    uint8_t no_seating_rows;
    uint8_t spinning_inertia;
    uint8_t spinning_friction;
    OpenRCT2::Audio::SoundId friction_sound_id;
    uint8_t log_flume_reverser_vehicle_type;
    uint8_t sound_range;
    uint8_t double_sound_frequency; // (Doubles the velocity when working out the sound frequency {used on go karts})
    uint8_t powered_acceleration;
    uint8_t powered_max_speed;
    uint8_t car_visual;
    uint8_t effect_visual;
    uint8_t draw_order;
    uint8_t num_vertical_frames_override; // A custom number that can be used rather than letting RCT2 determine it.
                                          // Needs the VEHICLE_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES flag to be set.
    uint8_t peep_loading_waypoint_segments;
    uint8_t pad_62[6] = {};
    std::vector<std::array<CoordsXY, 3>> peep_loading_waypoints = {};
    std::vector<int8_t> peep_loading_positions = {};
};
#ifdef __TESTPAINT__
#    pragma pack(pop)
#endif // __TESTPAINT__
#ifdef PLATFORM_32BIT
static_assert(sizeof(rct_ride_entry_vehicle) % 4 == 0, "Invalid struct size");
#else
static_assert(sizeof(rct_ride_entry_vehicle) % 8 == 0, "Invalid struct size");
#endif
