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

enum : uint32_t
{
    VEHICLE_ENTRY_FLAG_POWERED_RIDE_UNRESTRICTED_GRAVITY = 1
        << 0, // Set on powered vehicles that do not slow down when going down a hill.
    VEHICLE_ENTRY_FLAG_NO_UPSTOP_WHEELS = 1 << 1,
    VEHICLE_ENTRY_FLAG_NO_UPSTOP_BOBSLEIGH = 1 << 2,
    VEHICLE_ENTRY_FLAG_MINI_GOLF = 1 << 3,
    VEHICLE_ENTRY_FLAG_REVERSER_BOGIE = 1 << 4,
    VEHICLE_ENTRY_FLAG_REVERSER_PASSENGER_CAR = 1 << 5,
    VEHICLE_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET = 1 << 6, // Set on vehicles that support running inverted for extended periods
                                                         // of time, i.e. the Flying, Lay-down and Multi-dimension RCs.
    VEHICLE_ENTRY_FLAG_DODGEM_INUSE_LIGHTS = 1
        << 7, // When set the vehicle has an additional frame for when in use. Used only by dodgems.
    VEHICLE_ENTRY_FLAG_ALLOW_DOORS_DEPRECATED = 1 << 8, // Not used any more - every vehicle will now work with doors.
    VEHICLE_ENTRY_FLAG_ENABLE_TERNARY_COLOUR = 1 << 9,
    VEHICLE_ENTRY_FLAG_RECALCULATE_SPRITE_BOUNDS = 1 << 10, // Only used during loading of the objects.
    VEHICLE_ENTRY_FLAG_USE_16_ROTATION_FRAMES = 1
        << 11, // Instead of the default 32 rotation frames. Only used for boat hire and works only for non sloped sprites.
    VEHICLE_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES = 1
        << 12, // Setting this will cause the game to set vehicleEntry->num_vertical_frames to
               // vehicleEntry->num_vertical_frames_override, rather than determining it itself.
    VEHICLE_ENTRY_FLAG_SPRITE_BOUNDS_INCLUDE_INVERTED_SET = 1
        << 13, // Used together with HAS_INVERTED_SPRITE_SET and RECALCULATE_SPRITE_BOUNDS and includes the inverted sprites
               // into the function that recalculates the sprite bounds.
    VEHICLE_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES = 1
        << 14, // 16x additional frames for vehicle. A spinning item with additional frames must always face forward to
               // load/unload. Spinning without can load/unload at 4 rotations.
    VEHICLE_ENTRY_FLAG_LIFT = 1 << 15,
    VEHICLE_ENTRY_FLAG_ENABLE_TRIM_COLOUR = 1 << 16,
    VEHICLE_ENTRY_FLAG_SWINGING = 1 << 17,
    VEHICLE_ENTRY_FLAG_SPINNING = 1 << 18,
    VEHICLE_ENTRY_FLAG_POWERED = 1 << 19,
    VEHICLE_ENTRY_FLAG_RIDERS_SCREAM = 1 << 20,
    VEHICLE_ENTRY_FLAG_SUSPENDED_SWING = 1 << 21, // Suspended swinging coaster, or bobsleigh if SLIDE_SWING is also enabled.
    VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION = 1 << 22,
    VEHICLE_ENTRY_FLAG_VEHICLE_ANIMATION = 1 << 23, // Set on animated vehicles like the Multi-dimension coaster trains,
                                                    // Miniature Railway locomotives and Helicycles.
    VEHICLE_ENTRY_FLAG_RIDER_ANIMATION = 1 << 24,   // Set when the animation updates rider sprite positions.
    VEHICLE_ENTRY_FLAG_WOODEN_WILD_MOUSE_SWING = 1 << 25,
    VEHICLE_ENTRY_FLAG_LOADING_WAYPOINTS = 1
        << 26, // Peep loading positions have x and y coordinates. Normal rides just have offsets.
    VEHICLE_ENTRY_FLAG_SLIDE_SWING = 1
        << 27, // Set on dingy slides. They have there own swing value calculations and have a different amount of images.
               // Also set on bobsleighs together with the SUSPENDED_SWING flag.
    VEHICLE_ENTRY_FLAG_CHAIRLIFT = 1 << 28,
    VEHICLE_ENTRY_FLAG_WATER_RIDE = 1 << 29, // Set on rides where water would provide continuous propulsion.
    VEHICLE_ENTRY_FLAG_GO_KART = 1 << 30,
    VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT = 1u << 31,
};

enum : uint32_t
{
    VEHICLE_SPRITE_FLAG_FLAT = (1 << 0),
    VEHICLE_SPRITE_FLAG_GENTLE_SLOPES = (1 << 1),
    VEHICLE_SPRITE_FLAG_STEEP_SLOPES = (1 << 2),
    VEHICLE_SPRITE_FLAG_VERTICAL_SLOPES = (1 << 3),
    VEHICLE_SPRITE_FLAG_DIAGONAL_SLOPES = (1 << 4),
    VEHICLE_SPRITE_FLAG_FLAT_BANKED = (1 << 5),
    VEHICLE_SPRITE_FLAG_INLINE_TWISTS = (1 << 6),
    VEHICLE_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_BANKED_TRANSITIONS = (1 << 7),
    VEHICLE_SPRITE_FLAG_DIAGONAL_GENTLE_SLOPE_BANKED_TRANSITIONS = (1 << 8),
    VEHICLE_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TRANSITIONS = (1 << 9),
    VEHICLE_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TURNS = (1 << 10),
    VEHICLE_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_WHILE_BANKED_TRANSITIONS = (1 << 11),
    VEHICLE_SPRITE_FLAG_CORKSCREWS = (1 << 12),
    VEHICLE_SPRITE_FLAG_RESTRAINT_ANIMATION = (1 << 13),
    VEHICLE_SPRITE_FLAG_CURVED_LIFT_HILL = (1 << 14),
    // Used only on lifts (the transport ride), to only use 4 rotation sprites instead of 32.
    VEHICLE_SPRITE_FLAG_USE_4_ROTATION_FRAMES = (1 << 15),
};

/**
 * Ride type vehicle structure.
 * size: 0x65
 */
struct rct_ride_entry_vehicle
{
    uint16_t rotation_frame_mask;
    uint8_t NumRotationFrames;
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

    constexpr uint32_t GetNumRotationFrames() const
    {
        if (flags & VEHICLE_ENTRY_FLAG_USE_16_ROTATION_FRAMES)
            return 16;
        if (sprite_flags & VEHICLE_SPRITE_FLAG_USE_4_ROTATION_FRAMES)
            return 4;

        return 32;
    }
};
