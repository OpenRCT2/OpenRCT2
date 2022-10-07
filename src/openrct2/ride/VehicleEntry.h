/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../entity/Yaw.hpp"
#include "../util/Util.h"
#include "../world/Location.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace OpenRCT2::Audio
{
    enum class SoundId : uint8_t;
}

enum : uint32_t
{
    CAR_ENTRY_FLAG_POWERED_RIDE_UNRESTRICTED_GRAVITY = 1
        << 0, // Set on powered vehicles that do not slow down when going down a hill.
    CAR_ENTRY_FLAG_NO_UPSTOP_WHEELS = 1 << 1,
    CAR_ENTRY_FLAG_NO_UPSTOP_BOBSLEIGH = 1 << 2,
    CAR_ENTRY_FLAG_MINI_GOLF = 1 << 3,
    CAR_ENTRY_FLAG_REVERSER_BOGIE = 1 << 4,
    CAR_ENTRY_FLAG_REVERSER_PASSENGER_CAR = 1 << 5,
    CAR_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET = 1 << 6, // Set on vehicles that support running inverted for extended periods
                                                     // of time, i.e. the Flying, Lay-down and Multi-dimension RCs.
    CAR_ENTRY_FLAG_DODGEM_INUSE_LIGHTS = 1
        << 7, // When set the vehicle has an additional frame for when in use. Used only by dodgems.
    CAR_ENTRY_FLAG_ALLOW_DOORS_DEPRECATED = 1 << 8, // Not used any more - every vehicle will now work with doors.
    CAR_ENTRY_FLAG_ENABLE_TERNARY_COLOUR = 1 << 9,
    CAR_ENTRY_FLAG_RECALCULATE_SPRITE_BOUNDS = 1 << 10, // Only used during loading of the objects.
    CAR_ENTRY_FLAG_USE_16_ROTATION_FRAMES = 1
        << 11, // Instead of the default 32 rotation frames. Only used for boat hire and works only for non sloped sprites.
    CAR_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES = 1
        << 12, // Setting this will cause the game to set carEntry->num_vertical_frames to
               // carEntry->num_vertical_frames_override, rather than determining it itself.
    CAR_ENTRY_FLAG_SPRITE_BOUNDS_INCLUDE_INVERTED_SET = 1
        << 13, // Used together with HAS_INVERTED_SPRITE_SET and RECALCULATE_SPRITE_BOUNDS and includes the inverted sprites
               // into the function that recalculates the sprite bounds.
    CAR_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES = 1
        << 14, // 16x additional frames for vehicle. A spinning item with additional frames must always face forward to
               // load/unload. Spinning without can load/unload at 4 rotations.
    CAR_ENTRY_FLAG_LIFT = 1 << 15,
    CAR_ENTRY_FLAG_ENABLE_TRIM_COLOUR = 1 << 16,
    CAR_ENTRY_FLAG_SWINGING = 1 << 17,
    CAR_ENTRY_FLAG_SPINNING = 1 << 18,
    CAR_ENTRY_FLAG_POWERED = 1 << 19,
    CAR_ENTRY_FLAG_RIDERS_SCREAM = 1 << 20,
    CAR_ENTRY_FLAG_SUSPENDED_SWING = 1 << 21, // Suspended swinging coaster, or bobsleigh if SLIDE_SWING is also enabled.
    CAR_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION = 1 << 22,
    CAR_ENTRY_FLAG_VEHICLE_ANIMATION = 1 << 23, // Set on animated vehicles like the Multi-dimension coaster trains,
                                                // Miniature Railway locomotives and Helicycles.
    CAR_ENTRY_FLAG_RIDER_ANIMATION = 1 << 24,   // Set when the animation updates rider sprite positions.
    CAR_ENTRY_FLAG_WOODEN_WILD_MOUSE_SWING = 1 << 25,
    CAR_ENTRY_FLAG_LOADING_WAYPOINTS = 1
        << 26, // Peep loading positions have x and y coordinates. Normal rides just have offsets.
    CAR_ENTRY_FLAG_SLIDE_SWING = 1
        << 27, // Set on dingy slides. They have there own swing value calculations and have a different amount of images.
               // Also set on bobsleighs together with the SUSPENDED_SWING flag.
    CAR_ENTRY_FLAG_CHAIRLIFT = 1 << 28,
    CAR_ENTRY_FLAG_WATER_RIDE = 1 << 29, // Set on rides where water would provide continuous propulsion.
    CAR_ENTRY_FLAG_GO_KART = 1 << 30,
    CAR_ENTRY_FLAG_DODGEM_CAR_PLACEMENT = 1u << 31,
};

enum : uint32_t
{
    CAR_SPRITE_FLAG_FLAT = (1 << 0),
    CAR_SPRITE_FLAG_GENTLE_SLOPES = (1 << 1),
    CAR_SPRITE_FLAG_STEEP_SLOPES = (1 << 2),
    CAR_SPRITE_FLAG_VERTICAL_SLOPES = (1 << 3),
    CAR_SPRITE_FLAG_DIAGONAL_SLOPES = (1 << 4),
    CAR_SPRITE_FLAG_FLAT_BANKED = (1 << 5),
    CAR_SPRITE_FLAG_INLINE_TWISTS = (1 << 6),
    CAR_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_BANKED_TRANSITIONS = (1 << 7),
    CAR_SPRITE_FLAG_DIAGONAL_GENTLE_SLOPE_BANKED_TRANSITIONS = (1 << 8),
    CAR_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TRANSITIONS = (1 << 9),
    CAR_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TURNS = (1 << 10),
    CAR_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_WHILE_BANKED_TRANSITIONS = (1 << 11),
    CAR_SPRITE_FLAG_CORKSCREWS = (1 << 12),
    CAR_SPRITE_FLAG_RESTRAINT_ANIMATION = (1 << 13),
    CAR_SPRITE_FLAG_CURVED_LIFT_HILL = (1 << 14),
    // Used only on lifts (the transport ride), to only use 4 rotation sprites instead of 32.
    CAR_SPRITE_FLAG_USE_4_ROTATION_FRAMES = (1 << 15),
};

enum class SpriteGroupType : uint8_t
{
    SlopeFlat = 0,
    Slopes12,
    Slopes25,
    Slopes42,
    Slopes60,
    Slopes75,
    Slopes90,
    SlopesLoop,
    SlopeInverted,
    Slopes8,
    Slopes16,
    Slopes50,
    FlatBanked22,
    FlatBanked45,
    FlatBanked67,
    FlatBanked90,
    InlineTwists,
    Slopes12Banked22,
    Slopes8Banked22,
    Slopes25Banked22,
    Slopes25Banked45,
    Slopes12Banked45,
    Slopes25Banked67,
    Slopes25Banked90,
    Slopes25InlineTwists,
    Slopes42Banked22,
    Slopes42Banked45,
    Slopes42Banked67,
    Slopes42Banked90,
    Slopes60Banked22,
    Corkscrews,
    RestraintAnimation,
    CurvedLiftHill,
    Count
};

static const std::string SpriteGroupNames[] = {
    "slopeFlat",        "slopes12",           "slopes25",         "slopes42",         "slopes60",
    "slopes75",         "slopes90",           "slopesLoop",       "slopeInverted",    "slopes8",
    "slopes16",         "slopes50",           "flatBanked22",     "flatBanked45",     "flatBanked67",
    "flatBanked90",     "inlineTwists",       "slopes12Banked22", "slopes8Banked22",  "slopes25Banked22",
    "slopes25Banked45", "slopes12Banked45",   "slopes25Banked67", "slopes25Banked90", "slopes25InlineTwists",
    "slopes42Banked22", "slopes42Banked45",   "slopes42Banked67", "slopes42Banked90", "slopes60Banked22",
    "corkscrews",       "restraintAnimation", "curvedLiftHill",
};
static_assert(std::size(SpriteGroupNames) == EnumValue(SpriteGroupType::Count));

struct VehicleSpriteGroup
{
    uint32_t imageId{};
    OpenRCT2::Entity::Yaw::SpritePrecision spritePrecision{};
    bool Enabled() const
    {
        return spritePrecision != OpenRCT2::Entity::Yaw::SpritePrecision::None;
    }
};

/**
 * Ride type vehicle structure.
 */
struct CarEntry
{
    uint16_t TabRotationMask;
    uint32_t spacing;
    uint16_t car_mass;
    int8_t tab_height;
    uint8_t num_seats;
    uint8_t sprite_width;
    uint8_t sprite_height_negative;
    uint8_t sprite_height_positive;
    uint8_t animation;
    uint32_t flags;
    uint16_t base_num_frames; // The number of sprites of animation or swinging per rotation frame
    uint32_t base_image_id;
    VehicleSpriteGroup SpriteGroups[EnumValue(SpriteGroupType::Count)];
    uint32_t NumCarImages;
    uint8_t no_seating_rows;
    uint8_t spinning_inertia;
    uint8_t spinning_friction;
    OpenRCT2::Audio::SoundId friction_sound_id;
    uint8_t ReversedCarIndex; // When the car is reversed (using a turntable or reverser), it will be changed to this car.
    uint8_t sound_range;
    uint8_t double_sound_frequency; // (Doubles the velocity when working out the sound frequency {used on go karts})
    uint8_t powered_acceleration;
    uint8_t powered_max_speed;
    uint8_t PaintStyle;
    uint8_t effect_visual;
    uint8_t draw_order;
    uint8_t num_vertical_frames_override; // A custom number that can be used rather than letting RCT2 determine it.
                                          // Needs the CAR_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES flag to be set.
    uint8_t peep_loading_waypoint_segments;
    std::vector<std::array<CoordsXY, 3>> peep_loading_waypoints = {};
    std::vector<int8_t> peep_loading_positions = {};

    uint32_t NumRotationSprites(SpriteGroupType rotationType) const;
    int32_t SpriteByYaw(int32_t yaw, SpriteGroupType rotationType) const;
    bool GroupEnabled(SpriteGroupType rotationType) const;
    uint32_t GroupImageId(SpriteGroupType spriteGroup) const;
    uint32_t SpriteOffset(SpriteGroupType spriteGroup, int32_t imageDirection, uint8_t rankIndex) const;
};
