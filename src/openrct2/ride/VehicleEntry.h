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
    uint16_t rotation_frame_mask;   // 0x00 , 0x1A
    uint8_t num_vertical_frames;    // 0x02 , 0x1C, Appears to be unused, except as a temporary variable in RCT2 (not needed for
                                    // OpenRCT2)
    uint8_t num_horizontal_frames;  // 0x03 , 0x1D, Appears to be unused, except as a temporary variable in RCT2 (not needed for
                                    // OpenRCT2)
    uint32_t spacing;               // 0x04 , 0x1E
    uint16_t car_mass;              // 0x08 , 0x22
    int8_t tab_height;              // 0x0A , 0x24
    uint8_t num_seats;              // 0x0B , 0x25
    uint16_t sprite_flags;          // 0x0C , 0x26
    uint8_t sprite_width;           // 0x0E , 0x28
    uint8_t sprite_height_negative; // 0x0F , 0x29
    uint8_t sprite_height_positive; // 0x10 , 0x2A
    uint8_t animation;              // 0x11 , 0x2B
    uint32_t flags;                 // 0x12 , 0x2C
    uint16_t base_num_frames;       // 0x16 , 0x30, The number of sprites for a flat non-banked track piece.
    uint32_t base_image_id;         // 0x18 , 0x32, Following image_id's populated during loading
    uint32_t restraint_image_id;    // 0x1C , 0x36
    uint32_t gentle_slope_image_id; // 0x20 , 0x3A
    uint32_t steep_slope_image_id;  // 0x24 , 0x3E
    uint32_t vertical_slope_image_id;                // 0x28 , 0x42
    uint32_t diagonal_slope_image_id;                // 0x2C , 0x46
    uint32_t banked_image_id;                        // 0x30 , 0x4A
    uint32_t inline_twist_image_id;                  // 0x34 , 0x4E
    uint32_t flat_to_gentle_bank_image_id;           // 0x38 , 0x52
    uint32_t diagonal_to_gentle_slope_bank_image_id; // 0x3C , 0x56
    uint32_t gentle_slope_to_bank_image_id;          // 0x40 , 0x5A
    uint32_t gentle_slope_bank_turn_image_id;        // 0x44 , 0x5E
    uint32_t flat_bank_to_gentle_slope_image_id;     // 0x48 , 0x62
    union
    {
        uint32_t curved_lift_hill_image_id; // 0x4C , 0x66
        uint32_t corkscrew_image_id;        // 0x4C , 0x66
    };
    uint32_t no_vehicle_images;                 // 0x50 , 0x6A
    uint8_t no_seating_rows;                    // 0x54 , 0x6E
    uint8_t spinning_inertia;                   // 0x55 , 0x6F
    uint8_t spinning_friction;                  // 0x56 , 0x70
    OpenRCT2::Audio::SoundId friction_sound_id; // 0x57 , 0x71
    uint8_t log_flume_reverser_vehicle_type;    // 0x58 , 0x72
    uint8_t sound_range;                        // 0x59 , 0x73
    uint8_t
        double_sound_frequency;   // 0x5A , 0x74 (Doubles the velocity when working out the sound frequency {used on go karts})
    uint8_t powered_acceleration; // 0x5B , 0x75
    uint8_t powered_max_speed;    // 0x5C , 0x76
    uint8_t car_visual;           // 0x5D , 0x77
    uint8_t effect_visual;
    uint8_t draw_order;
    uint8_t num_vertical_frames_override;   // 0x60 , 0x7A, A custom number that can be used rather than letting RCT2 determine
                                            // it. Needs the VEHICLE_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES flag to be set.
    uint8_t peep_loading_waypoint_segments; // 0x61 new
    uint8_t pad_62[6] = {};                 // 0x62 , 0x7B
    std::vector<std::array<CoordsXY, 3>> peep_loading_waypoints = {};
    std::vector<int8_t> peep_loading_positions = {}; // previously 0x61 , 0x7B
};
#ifdef __TESTPAINT__
#    pragma pack(pop)
#endif // __TESTPAINT__
#ifdef PLATFORM_32BIT
static_assert(sizeof(rct_ride_entry_vehicle) % 4 == 0, "Invalid struct size");
#else
static_assert(sizeof(rct_ride_entry_vehicle) % 8 == 0, "Invalid struct size");
#endif
