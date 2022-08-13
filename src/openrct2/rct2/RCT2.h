/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/FileSystem.hpp"
#include "../rct12/RCT12.h"
#include "../ride/RideRatings.h"
#include "Limits.h"

#include <tuple>
#include <vector>

struct rct_ride_entry;
class ObjectList;
enum class EditorStep : uint8_t;

namespace RCT2
{
    constexpr const StringId RCT2_RIDE_STRING_START = 2;

    // clang-format off
    constexpr const uint16_t RCT2_OBJECT_ENTRY_COUNT =
        Limits::MaxRideObjects +
        Limits::MaxSmallSceneryObjects +
        Limits::MaxLargeSceneryObjects +
        Limits::MaxWallSceneryObjects +
        Limits::MaxBannerObjects +
        Limits::MaxPathObjects +
        Limits::MaxPathAdditionObjects +
        Limits::MaxScenereyGroupObjects +
        Limits::MaxParkEntranceObjects +
        Limits::MaxWaterObjects +
        Limits::MaxScenarioTextObjects;
    // clang-format on
    static_assert(RCT2_OBJECT_ENTRY_COUNT == 721);

    // clang-format off
    constexpr const int32_t rct2_object_entry_group_counts[] = {
        Limits::MaxRideObjects,
        Limits::MaxSmallSceneryObjects,
        Limits::MaxLargeSceneryObjects,
        Limits::MaxWallSceneryObjects,
        Limits::MaxBannerObjects,
        Limits::MaxPathObjects,
        Limits::MaxPathAdditionObjects,
        Limits::MaxScenereyGroupObjects,
        Limits::MaxParkEntranceObjects,
        Limits::MaxWaterObjects,
        Limits::MaxScenarioTextObjects,
    };
    // clang-format on

    enum class EntityListId : uint8_t
    {
        Count = 6,
    };
#pragma pack(push, 1)
    /**
     * Ride structure.
     * size: 0x0260
     */
    struct Ride
    {
        uint8_t type; // 0x000
        // pointer to static info. for example, wild mouse type is 0x36, subtype is
        // 0x4c.
        RCT12ObjectEntryIndex subtype;                                // 0x001
        uint16_t pad_002;                                             // 0x002
        uint8_t mode;                                                 // 0x004
        uint8_t colour_scheme_type;                                   // 0x005
        RCT12VehicleColour vehicle_colours[Limits::MaxTrainsPerRide]; // 0x006
        uint8_t pad_046[0x03]; // 0x046, Used to be track colours in RCT1 without expansions
        // 0 = closed, 1 = open, 2 = test
        uint8_t status; // 0x049
        StringId name;  // 0x04A
        union
        {
            uint32_t name_arguments; // 0x04C
            struct
            {
                StringId name_arguments_type_name; // 0x04C
                uint16_t name_arguments_number;    // 0x04E
            };
        };
        RCT12xy8 overall_view;                               // 0x050
        RCT12xy8 station_starts[Limits::MaxStationsPerRide]; // 0x052
        uint8_t station_heights[Limits::MaxStationsPerRide]; // 0x05A
        uint8_t station_length[Limits::MaxStationsPerRide];  // 0x05E
        uint8_t station_depart[Limits::MaxStationsPerRide];  // 0x062
        // ride->vehicle index for current train waiting for passengers
        // at station
        uint8_t train_at_station[Limits::MaxStationsPerRide];    // 0x066
        RCT12xy8 entrances[Limits::MaxStationsPerRide];          // 0x06A
        RCT12xy8 exits[Limits::MaxStationsPerRide];              // 0x072
        uint16_t last_peep_in_queue[Limits::MaxStationsPerRide]; // 0x07A
        uint8_t pad_082[Limits::MaxStationsPerRide];             // 0x082, Used to be number of peeps in queue in RCT1, but this
                                                                 // has moved.
        uint16_t vehicles[Limits::MaxTrainsPerRide];             // 0x086, Points to the first car in the train
        uint8_t depart_flags;                                    // 0x0C6

        // Not sure if these should be uint or sint.
        uint8_t num_stations;                // 0x0C7
        uint8_t num_vehicles;                // 0x0C8
        uint8_t num_cars_per_train;          // 0x0C9
        uint8_t proposed_num_vehicles;       // 0x0CA
        uint8_t proposed_num_cars_per_train; // 0x0CB
        uint8_t max_trains;                  // 0x0CC
        uint8_t min_max_cars_per_train;      // 0x0CD
        uint8_t min_waiting_time;            // 0x0CE
        uint8_t max_waiting_time;            // 0x0CF
        union
        {
            uint8_t operation_option; // 0x0D0
            uint8_t time_limit;       // 0x0D0
            uint8_t NumLaps;          // 0x0D0
            uint8_t launch_speed;     // 0x0D0
            uint8_t speed;            // 0x0D0
            uint8_t rotations;        // 0x0D0
        };

        uint8_t boat_hire_return_direction; // 0x0D1
        RCT12xy8 boat_hire_return_position; // 0x0D2
        uint8_t measurement_index;          // 0x0D4
        // bits 0 through 4 are the number of helix sections
        // bit 5: spinning tunnel, water splash, or rapids
        // bit 6: log reverser, waterfall
        // bit 7: whirlpool
        uint8_t special_track_elements; // 0x0D5
        uint8_t pad_0D6[2];             // 0x0D6
        // Divide this value by 29127 to get the human-readable max speed
        // (in RCT2, display_speed = (max_speed * 9) >> 18)
        int32_t max_speed;                          // 0x0D8
        int32_t average_speed;                      // 0x0DC
        uint8_t current_test_segment;               // 0x0E0
        uint8_t average_speed_test_timeout;         // 0x0E1
        uint8_t pad_0E2[0x2];                       // 0x0E2
        int32_t length[Limits::MaxStationsPerRide]; // 0x0E4
        uint16_t time[Limits::MaxStationsPerRide];  // 0x0F4
        fixed16_2dp max_positive_vertical_g;        // 0x0FC
        fixed16_2dp max_negative_vertical_g;        // 0x0FE
        fixed16_2dp max_lateral_g;                  // 0x100
        fixed16_2dp previous_vertical_g;            // 0x102
        fixed16_2dp previous_lateral_g;             // 0x104
        uint8_t pad_106[0x2];                       // 0x106
        uint32_t testing_flags;                     // 0x108
        // x y map location of the current track piece during a test
        // this is to prevent counting special tracks multiple times
        RCT12xy8 cur_test_track_location; // 0x10C
        // Next 3 variables are related (XXXX XYYY ZZZa aaaa)
        uint16_t turn_count_default; // 0x10E X = current turn count
        uint16_t turn_count_banked;  // 0x110
        uint16_t turn_count_sloped;  // 0x112 X = number turns > 3 elements
        union
        {
            uint8_t inversions; // 0x114 (???X XXXX)
            uint8_t holes;      // 0x114 (???X XXXX)
            // This is a very rough approximation of how much of the ride is undercover.
            // It reaches the maximum value of 7 at about 50% undercover and doesn't increase beyond that.
            uint8_t sheltered_eighths; // 0x114 (XXX?-????)
        };
        // Y is number of powered lifts, X is drops
        uint8_t drops;               // 0x115 (YYXX XXXX)
        uint8_t start_drop_height;   // 0x116
        uint8_t highest_drop_height; // 0x117
        int32_t sheltered_length;    // 0x118
        // Unused always 0? Should affect nausea
        uint16_t var_11C;               // 0x11C
        uint8_t num_sheltered_sections; // 0x11E (?abY YYYY)
        // see cur_test_track_location
        uint8_t cur_test_track_z; // 0x11F
        // Customer counter in the current 960 game tick (about 30 seconds) interval
        uint16_t cur_num_customers; // 0x120
        // Counts ticks to update customer intervals, resets each 960 game ticks.
        uint16_t num_customers_timeout; // 0x122
        // Customer count in the last 10 * 960 game ticks (sliding window)
        uint16_t num_customers[Limits::CustomerHistorySize]; // 0x124
        money16 price;                                       // 0x138
        RCT12xy8 chairlift_bullwheel_location[2];            // 0x13A
        uint8_t chairlift_bullwheel_z[2];                    // 0x13E
        union
        {
            RatingTuple ratings; // 0x140
            struct
            {
                ride_rating excitement; // 0x140
                ride_rating intensity;  // 0x142
                ride_rating nausea;     // 0x144
            };
        };
        uint16_t value;                        // 0x146
        uint16_t chairlift_bullwheel_rotation; // 0x148
        uint8_t satisfaction;                  // 0x14A
        uint8_t satisfaction_time_out;         // 0x14B
        uint8_t satisfaction_next;             // 0x14C
        // Various flags stating whether a window needs to be refreshed
        uint8_t window_invalidate_flags; // 0x14D
        uint8_t pad_14E[0x02];           // 0x14E
        uint32_t total_customers;        // 0x150
        money32 total_profit;            // 0x154
        uint8_t popularity;              // 0x158
        uint8_t popularity_time_out;     // 0x159 Updated every purchase and ?possibly by time?
        uint8_t popularity_next;         // 0x15A When timeout reached this will be the next popularity
        uint8_t num_riders;              // 0x15B
        uint8_t music_tune_id;           // 0x15C
        uint8_t slide_in_use;            // 0x15D
        union
        {
            uint16_t slide_peep; // 0x15E
            uint16_t maze_tiles; // 0x15E
        };
        uint8_t pad_160[0xE];              // 0x160
        uint8_t slide_peep_t_shirt_colour; // 0x16E
        uint8_t pad_16F[0x7];              // 0x16F
        uint8_t spiral_slide_progress;     // 0x176
        uint8_t pad_177[0x9];              // 0x177
        int16_t build_date;                // 0x180
        money16 upkeep_cost;               // 0x182
        uint16_t race_winner;              // 0x184
        uint8_t pad_186[0x02];             // 0x186
        uint32_t music_position;           // 0x188
        uint8_t breakdown_reason_pending;  // 0x18C
        uint8_t mechanic_status;           // 0x18D
        uint16_t mechanic;                 // 0x18E
        uint8_t inspection_station;        // 0x190
        uint8_t broken_vehicle;            // 0x191
        uint8_t broken_car;                // 0x192
        uint8_t breakdown_reason;          // 0x193
        money16 price_secondary;           // 0x194
        union
        {
            struct
            {
                uint8_t reliability_subvalue;   // 0x196, 0 - 255, acts like the decimals for reliability_percentage
                uint8_t reliability_percentage; // 0x197, Starts at 100 and decreases from there.
            };
            uint16_t reliability; // 0x196
        };
        // Small constant used to increase the unreliability as the game continues,
        // making breakdowns more and more likely.
        uint8_t unreliability_factor; // 0x198
        // Range from [0, 100]
        uint8_t downtime;                                      // 0x199
        uint8_t inspection_interval;                           // 0x19A
        uint8_t last_inspection;                               // 0x19B
        uint8_t downtime_history[Limits::DowntimeHistorySize]; // 0x19C
        uint32_t no_primary_items_sold;                        // 0x1A4
        uint32_t no_secondary_items_sold;                      // 0x1A8
        uint8_t breakdown_sound_modifier;                      // 0x1AC
        // Used to oscillate the sound when ride breaks down.
        // 0 = no change, 255 = max change
        uint8_t not_fixed_timeout;                                  // 0x1AD
        uint8_t last_crash_type;                                    // 0x1AE
        uint8_t connected_message_throttle;                         // 0x1AF
        money32 income_per_hour;                                    // 0x1B0
        money32 profit;                                             // 0x1B4
        uint8_t queue_time[Limits::MaxStationsPerRide];             // 0x1B8
        uint8_t track_colour_main[Limits::NumColourSchemes];        // 0x1BC
        uint8_t track_colour_additional[Limits::NumColourSchemes];  // 0x1C0
        uint8_t track_colour_supports[Limits::NumColourSchemes];    // 0x1C4
        uint8_t music;                                              // 0x1C8
        uint8_t entrance_style;                                     // 0x1C9
        uint16_t vehicle_change_timeout;                            // 0x1CA
        uint8_t num_block_brakes;                                   // 0x1CC
        uint8_t lift_hill_speed;                                    // 0x1CD
        uint16_t guests_favourite;                                  // 0x1CE
        uint32_t lifecycle_flags;                                   // 0x1D0
        uint8_t vehicle_colours_extended[Limits::MaxTrainsPerRide]; // 0x1D4
        uint16_t total_air_time;                                    // 0x1F4
        uint8_t current_test_station;                               // 0x1F6
        uint8_t num_circuits;                                       // 0x1F7
        int16_t cable_lift_x;                                       // 0x1F8
        int16_t cable_lift_y;                                       // 0x1FA
        uint8_t cable_lift_z;                                       // 0x1FC
        uint8_t pad_1FD;                                            // 0x1FD
        uint16_t cable_lift;                                        // 0x1FE
        uint16_t queue_length[Limits::MaxStationsPerRide];          // 0x200
        uint8_t pad_208[0x58];                                      // 0x208

        uint8_t GetMinCarsPerTrain() const;
        uint8_t GetMaxCarsPerTrain() const;
        void SetMinCarsPerTrain(uint8_t newValue);
        void SetMaxCarsPerTrain(uint8_t newValue);
    };
    assert_struct_size(Ride, 0x260);

    /* Track Entrance entry size: 0x06 */
    struct TD6EntranceElement
    {
        int8_t z;          // 0x00
        uint8_t direction; // 0x01
        int16_t x;         // 0x02
        int16_t y;         // 0x04
    };
    assert_struct_size(TD6EntranceElement, 0x06);

    /* Track Scenery entry  size: 0x16 */
    struct TD6SceneryElement
    {
        rct_object_entry scenery_object; // 0x00
        int8_t x;                        // 0x10
        int8_t y;                        // 0x11
        int8_t z;                        // 0x12
        uint8_t flags;                   // 0x13 direction quadrant tertiary colour
        uint8_t primary_colour;          // 0x14
        uint8_t secondary_colour;        // 0x15
    };
    assert_struct_size(TD6SceneryElement, 0x16);

    /**
     * Track design structure.
     * size: 0xA3
     */
    struct TD6Track
    {
        uint8_t type; // 0x00
        RCT12ObjectEntryIndex vehicle_type;
        union
        {
            // After loading the track this is converted to
            // a cost but before its a flags register
            money32 cost;   // 0x02
            uint32_t flags; // 0x02
        };
        union
        {
            // After loading the track this is converted to
            // a flags register
            uint8_t ride_mode;   // 0x06
            uint8_t track_flags; // 0x06
        };
        uint8_t version_and_colour_scheme;                            // 0x07 0b0000_VVCC
        RCT12VehicleColour vehicle_colours[Limits::MaxTrainsPerRide]; // 0x08
        union
        {
            uint8_t pad_48;
            uint8_t track_spine_colour_rct1; // 0x48
        };
        union
        {
            uint8_t entrance_style;         // 0x49
            uint8_t track_rail_colour_rct1; // 0x49
        };
        union
        {
            uint8_t total_air_time;            // 0x4A
            uint8_t track_support_colour_rct1; // 0x4A
        };
        uint8_t depart_flags;             // 0x4B
        uint8_t number_of_trains;         // 0x4C
        uint8_t number_of_cars_per_train; // 0x4D
        uint8_t min_waiting_time;         // 0x4E
        uint8_t max_waiting_time;         // 0x4F
        uint8_t operation_setting;
        int8_t max_speed;                // 0x51
        int8_t average_speed;            // 0x52
        uint16_t ride_length;            // 0x53
        uint8_t max_positive_vertical_g; // 0x55
        int8_t max_negative_vertical_g;  // 0x56
        uint8_t max_lateral_g;           // 0x57
        union
        {
            uint8_t inversions; // 0x58
            uint8_t holes;      // 0x58
        };
        uint8_t drops;                                               // 0x59
        uint8_t highest_drop_height;                                 // 0x5A
        uint8_t excitement;                                          // 0x5B
        uint8_t intensity;                                           // 0x5C
        uint8_t nausea;                                              // 0x5D
        money16 upkeep_cost;                                         // 0x5E
        uint8_t track_spine_colour[Limits::NumColourSchemes];        // 0x60
        uint8_t track_rail_colour[Limits::NumColourSchemes];         // 0x64
        uint8_t track_support_colour[Limits::NumColourSchemes];      // 0x68
        uint32_t flags2;                                             // 0x6C
        rct_object_entry vehicle_object;                             // 0x70
        uint8_t space_required_x;                                    // 0x80
        uint8_t space_required_y;                                    // 0x81
        uint8_t vehicle_additional_colour[Limits::MaxTrainsPerRide]; // 0x82
        uint8_t lift_hill_speed_num_circuits;                        // 0xA2 0bCCCL_LLLL
        // 0xA3 (data starts here in file)
    };
    assert_struct_size(TD6Track, 0xA3);

    /**
     * scores.dat file header.
     * size: 0x10
     */
    struct ScoresHeader
    {
        uint32_t var_0;
        uint32_t var_4;
        uint32_t var_8;
        uint32_t ScenarioCount;
    };
    assert_struct_size(ScoresHeader, 0x10);

    /**
     * An entry of scores.dat
     * size: 0x02B0
     */
    struct ScoresEntry
    {
        char Path[256];
        uint8_t Category;
        uint8_t pad_0101[0x1F];
        int8_t ObjectiveType;
        int8_t ObjectiveArg1;
        int32_t objectiveArg2;
        int16_t objectiveArg3;
        char Name[64];
        char Details[256];
        int32_t Flags;
        money32 CompanyValue;
        char CompletedBy[64];
    };
    assert_struct_size(ScoresEntry, 0x02B0);

    struct Vehicle : RCT12SpriteBase
    {
        uint8_t Pitch;         // 0x1F
        uint8_t bank_rotation; // 0x20
        uint8_t pad_21[3];
        int32_t remaining_distance; // 0x24
        int32_t velocity;           // 0x28
        int32_t acceleration;       // 0x2C
        uint8_t ride;               // 0x30
        uint8_t vehicle_type;       // 0x31
        RCT12VehicleColour colours; // 0x32
        union
        {
            uint16_t track_progress; // 0x34
            struct
            {
                int8_t var_34;
                uint8_t var_35;
            };
        };
        union
        {
            int16_t TrackTypeAndDirection; // 0x36
            RCT12xy8 boat_location;        // 0x36
        };
        uint16_t track_x;               // 0x38
        uint16_t track_y;               // 0x3A
        uint16_t track_z;               // 0x3C
        uint16_t next_vehicle_on_train; // 0x3E
        uint16_t prev_vehicle_on_ride;  // 0x40
        uint16_t next_vehicle_on_ride;  // 0x42
        uint16_t var_44;
        uint16_t mass;         // 0x46
        uint16_t update_flags; // 0x48
        uint8_t SwingSprite;
        uint8_t current_station; // 0x4B
        union
        {
            int16_t SwingPosition; // 0x4C
            int16_t current_time;  // 0x4C
            struct
            {
                int8_t ferris_wheel_var_0; // 0x4C
                int8_t ferris_wheel_var_1; // 0x4D
            };
        };
        union
        {
            int16_t SwingSpeed;
            int16_t crash_z; // 0x4E
        };
        uint8_t status;                  // 0x50
        uint8_t sub_state;               // 0x51
        uint16_t peep[32];               // 0x52
        uint8_t peep_tshirt_colours[32]; // 0x92
        uint8_t num_seats;               // 0xB2
        uint8_t num_peeps;               // 0xB3
        uint8_t next_free_seat;          // 0xB4
        uint8_t restraints_position;     // 0xB5
        union
        {
            int16_t spin_speed; // 0xB6
            int16_t crash_x;    // 0xB6
        };
        uint16_t sound2_flags; // 0xB8
        uint8_t spin_sprite;   // 0xBA
        uint8_t sound1_id;     // 0xBB
        uint8_t sound1_volume; // 0xBC
        uint8_t sound2_id;     // 0xBD
        uint8_t sound2_volume; // 0xBE
        int8_t sound_vector_factor;
        union
        {
            uint16_t var_C0;
            int16_t crash_y;            // 0xC0
            uint16_t time_waiting;      // 0xC0
            uint16_t cable_lift_target; // 0xC0
        };
        uint8_t speed;                // 0xC2
        uint8_t powered_acceleration; // 0xC3
        union
        {
            uint8_t DodgemsCollisionDirection; // 0xC4
            uint8_t CollisionDetectionTimer;   // 0xC4
        };
        uint8_t animation_frame; // 0xC5
        uint8_t pad_C6[0x2];
        uint32_t animationState;
        uint8_t scream_sound_id; // 0xCC
        uint8_t TrackSubposition;
        uint8_t NumLaps;                // 0xCE
        uint8_t brake_speed;            // 0xCF
        uint16_t lost_time_out;         // 0xD0
        int8_t vertical_drop_countdown; // 0xD1
        uint8_t var_D3;
        uint8_t mini_golf_current_animation;
        uint8_t mini_golf_flags;      // 0xD5
        uint8_t ride_subtype;         // 0xD6
        uint8_t colours_extended;     // 0xD7
        uint8_t seat_rotation;        // 0xD8
        uint8_t target_seat_rotation; // 0xD9

        uint16_t GetTrackType() const
        {
            return TrackTypeAndDirection >> 2;
        }
        uint8_t GetTrackDirection() const
        {
            return TrackTypeAndDirection & RCT12VehicleTrackDirectionMask;
        }
        void SetTrackType(uint16_t trackType)
        {
            // set the upper 14 bits to 0
            TrackTypeAndDirection &= ~RCT12VehicleTrackTypeMask;
            TrackTypeAndDirection |= trackType << 2;
        }
        void SetTrackDirection(uint8_t trackDirection)
        {
            // set the lower 2 bits only
            TrackTypeAndDirection &= ~RCT12VehicleTrackDirectionMask;
            TrackTypeAndDirection |= trackDirection & RCT12VehicleTrackDirectionMask;
        }
    };
    assert_struct_size(Vehicle, 0xDA);

    struct Peep : RCT12SpriteBase
    {
        uint8_t pad_1F[0x22 - 0x1F];
        StringId name_string_idx; // 0x22
        uint16_t next_x;          // 0x24
        uint16_t next_y;          // 0x26
        uint8_t next_z;           // 0x28
        uint8_t next_flags;       // 0x29
        uint8_t outside_of_park;  // 0x2A
        uint8_t state;            // 0x2B
        uint8_t sub_state;        // 0x2C
        uint8_t sprite_type;      // 0x2D
        uint8_t peep_type;        // 0x2E
        union
        {
            uint8_t staff_type;  // 0x2F
            uint8_t no_of_rides; // 0x2F
        };
        uint8_t tshirt_colour;         // 0x30
        uint8_t trousers_colour;       // 0x31
        uint16_t destination_x;        // 0x32
        uint16_t destination_y;        // 0x34
        uint8_t destination_tolerance; // 0x36
        uint8_t var_37;
        uint8_t energy;                  // 0x38
        uint8_t energy_target;           // 0x39
        uint8_t happiness;               // 0x3A
        uint8_t happiness_target;        // 0x3B
        uint8_t nausea;                  // 0x3C
        uint8_t nausea_target;           // 0x3D
        uint8_t hunger;                  // 0x3E
        uint8_t thirst;                  // 0x3F
        uint8_t toilet;                  // 0x40
        uint8_t mass;                    // 0x41
        uint8_t time_to_consume;         // 0x42
        uint8_t intensity;               // 0x43
        uint8_t nausea_tolerance;        // 0x44
        uint8_t window_invalidate_flags; // 0x45
        money16 paid_on_drink;           // 0x46
        uint8_t ride_types_been_on[16];  // 0x48
        uint32_t item_extra_flags;       // 0x58
        RCT12RideId photo2_ride_ref;     // 0x5C
        RCT12RideId photo3_ride_ref;     // 0x5D
        RCT12RideId photo4_ride_ref;     // 0x5E
        uint8_t pad_5F[0x09];            // 0x5F
        RCT12RideId current_ride;        // 0x68
        uint8_t current_ride_station;    // 0x69
        uint8_t current_train;           // 0x6A
        union
        {
            struct
            {
                uint8_t current_car;  // 0x6B
                uint8_t current_seat; // 0x6C
            };
            uint16_t time_to_sitdown; // 0x6B
            struct
            {
                uint8_t time_to_stand;  // 0x6B
                uint8_t standing_flags; // 0x6C
            };
        };
        uint8_t special_sprite;             // 0x6D
        uint8_t action_sprite_type;         // 0x6E
        uint8_t next_action_sprite_type;    // 0x6F
        uint8_t action_sprite_image_offset; // 0x70
        uint8_t action;                     // 0x71
        uint8_t action_frame;               // 0x72
        uint8_t step_progress;              // 0x73
        union
        {
            uint16_t mechanic_time_since_call;
            uint16_t next_in_queue; // 0x74
        };
        uint8_t pad_76;
        uint8_t pad_77;
        union
        {
            uint8_t maze_last_edge; // 0x78
            uint8_t direction;
        };
        RCT12RideId interaction_ride_index;
        uint16_t time_in_queue;                             // 0x7A
        uint8_t rides_been_on[32];                          // 0x7C
        uint32_t id;                                        // 0x9C
        money32 cash_in_pocket;                             // 0xA0
        money32 cash_spent;                                 // 0xA4
        int32_t park_entry_time;                            // 0xA8
        int8_t rejoin_queue_timeout;                        // 0xAC
        RCT12RideId previous_ride;                          // 0xAD
        uint16_t previous_ride_time_out;                    // 0xAE
        RCT12PeepThought thoughts[Limits::MaxPeepThoughts]; // 0xB0
        uint8_t path_check_optimisation;                    // 0xC4
        union
        {
            uint8_t staff_id;                     // 0xC5
            RCT12RideId guest_heading_to_ride_id; // 0xC5
        };
        union
        {
            uint8_t staff_orders;           // 0xC6
            uint8_t peep_is_lost_countdown; // 0xC6
        };
        RCT12RideId photo1_ride_ref;     // 0xC7
        uint32_t peep_flags;             // 0xC8
        rct12_xyzd8 pathfind_goal;       // 0xCC
        rct12_xyzd8 pathfind_history[4]; // 0xD0
        uint8_t no_action_frame_num;     // 0xE0
        uint8_t litter_count;            // 0xE1
        union
        {
            uint8_t time_on_ride;         // 0xE2
            uint8_t staff_mowing_timeout; // 0xE2
        };
        uint8_t disgusting_count; // 0xE3
        union
        {
            money16 paid_to_enter;      // 0xE4
            uint16_t staff_lawns_mown;  // 0xE4
            uint16_t staff_rides_fixed; // 0xE4
        };
        union
        {
            money16 paid_on_rides;          // 0xE6
            uint16_t staff_gardens_watered; // 0xE6
            uint16_t staff_rides_inspected; // 0xE6
        };
        union
        {
            money16 paid_on_food;        // 0xE8
            uint16_t staff_litter_swept; // 0xE8
        };
        union
        {
            money16 paid_on_souvenirs;   // 0xEA
            uint16_t staff_bins_emptied; // 0xEA
        };
        uint8_t no_of_food;                   // 0xEC
        uint8_t no_of_drinks;                 // 0xED
        uint8_t no_of_souvenirs;              // 0xEE
        uint8_t vandalism_seen;               // 0xEF 0xC0 vandalism thought timeout, 0x3F vandalism tiles seen
        uint8_t voucher_type;                 // 0xF0
        RCT12RideId voucher_arguments;        // 0xF1 ride_id or string_offset_id
        uint8_t surroundings_thought_timeout; // 0xF2
        uint8_t angriness;                    // 0xF3
        uint8_t time_lost;             // 0xF4 the time the peep has been lost when it reaches 254 generates the lost thought
        uint8_t days_in_queue;         // 0xF5
        uint8_t balloon_colour;        // 0xF6
        uint8_t umbrella_colour;       // 0xF7
        uint8_t hat_colour;            // 0xF8
        RCT12RideId favourite_ride;    // 0xF9
        uint8_t favourite_ride_rating; // 0xFA
        uint8_t pad_FB;
        uint32_t item_standard_flags; // 0xFC
        uint64_t GetItemFlags() const
        {
            return item_standard_flags | (static_cast<uint64_t>(item_extra_flags) << 32);
        }
    };
    assert_struct_size(Peep, 0x100);

    enum class StaffMode : uint8_t
    {
        None,
        Walk,
        Patrol = 3
    };

    union Entity
    {
    private:
        uint8_t pad_00[0x100];

    public:
        RCT12SpriteBase unknown;
        Vehicle vehicle;
        Peep peep;
        RCT12SpriteLitter litter;
        RCT12SpriteBalloon balloon;
        RCT12SpriteDuck duck;
        RCT12SpriteJumpingFountain jumping_fountain;
        RCT12SpriteMoneyEffect money_effect;
        RCT12SpriteCrashedVehicleParticle crashed_vehicle_particle;
        RCT12SpriteCrashSplash crash_splash;
        RCT12SpriteSteamParticle steam_particle;
        RCT12SpriteParticle misc_particle;
    };
    assert_struct_size(Entity, 0x100);

    struct RideRatingCalculationData
    {
        uint16_t proximity_x;
        uint16_t proximity_y;
        uint16_t proximity_z;
        uint16_t proximity_start_x;
        uint16_t proximity_start_y;
        uint16_t proximity_start_z;
        uint8_t current_ride;
        uint8_t state;
        uint8_t proximity_track_type;
        uint8_t proximity_base_height;
        uint16_t proximity_total;
        uint16_t proximity_scores[26];
        uint16_t num_brakes;
        uint16_t num_reversers;
        uint16_t station_flags;
    };
    assert_struct_size(RideRatingCalculationData, 76);

    /**
     * SV6/SC6 header chunk
     * size: 0x20
     */
    struct S6Header
    {
        uint8_t type;                // 0x00
        uint8_t classic_flag;        // 0x01
        uint16_t num_packed_objects; // 0x02
        uint32_t version;            // 0x04
        uint32_t magic_number;       // 0x08
        uint8_t pad_0C[0x14];
    };
    assert_struct_size(S6Header, 0x20);

    /**
     * SC6 information chunk
     * size: 0x198
     */
    struct S6Info
    {
        EditorStep editor_step;
        uint8_t category;        // 0x01
        uint8_t objective_type;  // 0x02
        uint8_t objective_arg_1; // 0x03
        int32_t objective_arg_2; // 0x04
        int16_t objective_arg_3; // 0x08
        uint8_t pad_00A[0x3E];
        char name[64];          // 0x48
        char details[256];      // 0x88
        rct_object_entry entry; // 0x188
    };
    assert_struct_size(S6Info, 0x198);

    struct S6Data
    {
        // SC6[0]
        S6Header header;

        // SC6[1]
        S6Info info;

        // SC6[2]
        // packed objects

        // SC6[3]
        union
        {
            rct_object_entry Objects[RCT2_OBJECT_ENTRY_COUNT];
            struct
            {
                rct_object_entry RideObjects[Limits::MaxRideObjects];
                rct_object_entry SceneryObjects[Limits::MaxSmallSceneryObjects];
                rct_object_entry LargeSceneryObjects[Limits::MaxLargeSceneryObjects];
                rct_object_entry WallSceneryObjects[Limits::MaxWallSceneryObjects];
                rct_object_entry BannerObjects[Limits::MaxBannerObjects];
                rct_object_entry PathObjects[Limits::MaxPathObjects];
                rct_object_entry PathAdditionObjects[Limits::MaxPathAdditionObjects];
                rct_object_entry SceneryGroupObjects[Limits::MaxScenereyGroupObjects];
                rct_object_entry ParkEntranceObjects[Limits::MaxParkEntranceObjects];
                rct_object_entry WaterObjects[Limits::MaxWaterObjects];
                rct_object_entry ScenarioTextObjects[Limits::MaxScenarioTextObjects];
            };
        };

        // SC6[4]
        uint16_t elapsed_months;
        uint16_t current_day;
        uint32_t scenario_ticks;
        uint32_t scenario_srand_0;
        uint32_t scenario_srand_1;

        // SC6[5]
        RCT12TileElement tile_elements[Limits::MaxTileElements];

        // SC6[6]
        uint32_t next_free_tile_element_pointer_index;
        Entity sprites[Limits::MaxEntities];
        uint16_t sprite_lists_head[static_cast<uint8_t>(EntityListId::Count)];
        uint16_t sprite_lists_count[static_cast<uint8_t>(EntityListId::Count)];
        StringId park_name;
        uint8_t pad_013573D6[2];
        uint32_t park_name_args;
        money32 initial_cash;
        money32 current_loan;
        uint32_t park_flags;
        money16 park_entrance_fee;
        uint16_t rct1_park_entrance_x;
        uint16_t rct1_park_entrance_y;
        uint8_t pad_013573EE[2];
        uint8_t rct1_park_entrance_z;
        uint8_t pad_013573F1;
        rct12_peep_spawn peep_spawns[Limits::MaxPeepSpawns];
        uint8_t guest_count_change_modifier;
        uint8_t current_research_level;
        uint8_t pad_01357400[4];
        uint32_t researched_ride_types[Limits::MaxResearchedRideTypeQuads];
        uint32_t researched_ride_entries[Limits::MaxResearchedRideEntryQuads];
        uint32_t researched_track_types_a[128];
        uint32_t researched_track_types_b[128];

        // SC6[7]
        uint16_t guests_in_park;
        uint16_t guests_heading_for_park;

        // Ignored in scenario
        money32 expenditure_table[Limits::ExpenditureTableMonthCount][Limits::ExpenditureTypeCount];

        // SC6[8]
        uint16_t last_guests_in_park;
        uint8_t pad_01357BCA[3];
        uint8_t handyman_colour;
        uint8_t mechanic_colour;
        uint8_t security_colour;

        // Ignored in scenario
        uint32_t researched_scenery_items[Limits::MaxResearchedSceneryItemQuads];

        // SC6[9]
        uint16_t park_rating;

        // Ignored in scenario
        uint8_t park_rating_history[32];
        uint8_t guests_in_park_history[32];

        // SC6[10]
        uint8_t active_research_types;
        uint8_t research_progress_stage;
        uint32_t last_researched_item_subject;
        uint8_t pad_01357CF8[1000];
        uint32_t next_research_item;
        uint16_t research_progress;
        uint8_t next_research_category;
        uint8_t next_research_expected_day;
        uint8_t next_research_expected_month;
        uint8_t guest_initial_happiness;
        uint16_t park_size;
        uint16_t guest_generation_probability;
        uint16_t total_ride_value_for_money;
        money32 maximum_loan;
        money16 guest_initial_cash;
        uint8_t guest_initial_hunger;
        uint8_t guest_initial_thirst;
        uint8_t objective_type;
        uint8_t objective_year;
        uint8_t pad_013580FA[2];
        money32 objective_currency;
        uint16_t objective_guests;
        uint8_t campaign_weeks_left[20];
        uint8_t campaign_ride_index[22];

        // Ignored in scenario
        money32 balance_history[Limits::FinanceGraphSize];

        // SC6[11]
        money32 current_expenditure;
        money32 current_profit;
        money32 weekly_profit_average_dividend;
        uint16_t weekly_profit_average_divisor;
        uint8_t pad_0135833A[2];

        // Ignored in scenario
        money32 weekly_profit_history[Limits::FinanceGraphSize];

        // SC6[12]
        money32 park_value;

        // Ignored in scenario
        money32 park_value_history[Limits::FinanceGraphSize];

        // SC6[13]
        money32 completed_company_value;
        uint32_t total_admissions;
        money32 income_from_admissions;
        money32 company_value;
        uint8_t peep_warning_throttle[16];
        rct12_award awards[Limits::MaxAwards];
        money16 land_price;
        money16 construction_rights_price;
        uint16_t word_01358774;
        uint8_t pad_01358776[2];
        uint32_t cd_key;
        uint8_t pad_0135877C[64];
        uint32_t game_version_number;
        money32 completed_company_value_record;
        uint32_t loan_hash;
        uint16_t ride_count;
        uint8_t pad_013587CA[6];
        money32 historical_profit;
        uint8_t pad_013587D4[4];
        char scenario_completed_name[32];
        money32 cash;
        uint8_t pad_013587FC[50];
        uint16_t park_rating_casualty_penalty;
        uint16_t map_size_units;
        uint16_t map_size_minus_2;
        uint16_t map_size;
        uint16_t map_max_xy;
        uint32_t same_price_throughout;
        uint16_t suggested_max_guests;
        uint16_t park_rating_warning_days;
        uint8_t last_entrance_style;
        uint8_t rct1_water_colour;
        uint8_t pad_01358842[2];
        RCT12ResearchItem research_items[Limits::MaxResearchItems];
        uint16_t map_base_z;
        char scenario_name[64];
        char scenario_description[256];
        uint8_t current_interest_rate;
        uint8_t pad_0135934B;
        uint32_t same_price_throughout_extended;
        int16_t park_entrance_x[Limits::MaxParkEntrances];
        int16_t park_entrance_y[Limits::MaxParkEntrances];
        int16_t park_entrance_z[Limits::MaxParkEntrances];
        uint8_t park_entrance_direction[Limits::MaxParkEntrances];
        char scenario_filename[256];
        uint8_t saved_expansion_pack_names[3256];
        RCT12Banner banners[Limits::MaxBanners];
        char custom_strings[Limits::MaxUserStrings][Limits::MaxUserStringLength];
        uint32_t game_ticks_1;
        Ride rides[Limits::MaxRidesInPark];
        uint16_t saved_age;
        int16_t saved_view_x;
        int16_t saved_view_y;
        uint8_t saved_view_zoom;
        uint8_t saved_view_rotation;
        RCT12MapAnimation map_animations[Limits::MaxAnimatedObjects];
        uint16_t num_map_animations;
        uint8_t pad_0138B582[2];
        RideRatingCalculationData ride_ratings_calc_data;
        uint8_t pad_0138B5D0[60];
        RCT12RideMeasurement ride_measurements[8];
        uint32_t next_guest_index;
        uint16_t grass_and_scenery_tilepos;
        uint32_t patrol_areas[(Limits::MaxStaff + Limits::StaffTypeCount) * Limits::PatrolAreaSize];
        StaffMode staff_modes[Limits::MaxStaff + Limits::StaffTypeCount];
        uint8_t pad_13CA73E;
        uint8_t pad_13CA73F;
        uint8_t byte_13CA740;
        uint8_t pad_13CA741;
        uint8_t byte_13CA742[4]; // unused
        uint8_t climate;
        uint8_t pad_013CA747;
        uint16_t climate_update_timer;
        uint8_t current_weather;
        uint8_t next_weather;
        uint8_t temperature;
        uint8_t next_temperature;
        uint8_t current_weather_effect;
        uint8_t next_weather_effect;
        uint8_t current_weather_gloom;
        uint8_t next_weather_gloom;
        uint8_t current_weather_level;
        uint8_t next_weather_level;
        rct12_news_item news_items[Limits::MaxNewsItems];
        char rct1_scenario_name[62];       // Unused in RCT2
        uint16_t rct1_scenario_slot_index; // Unused in RCT2
        uint32_t rct1_scenario_flags;      // Unused in RCT2
        uint16_t wide_path_tile_loop_x;
        uint16_t wide_path_tile_loop_y;
        uint8_t pad_13CE778[434];
    };
    assert_struct_size(S6Data, 0x46b44a);

    struct StexEntry
    {
        StringId scenario_name; // 0x00
        StringId park_name;     // 0x02
        StringId details;       // 0x04
        uint8_t var_06;
    };
    assert_struct_size(StexEntry, 7);
#pragma pack(pop)

    ObjectEntryIndex RCT2RideTypeToOpenRCT2RideType(uint8_t rct2RideType, const rct_ride_entry* rideEntry);
    bool RCT2TrackTypeIsBooster(uint8_t rideType, uint16_t trackType);
    bool RCT2RideTypeNeedsConversion(uint8_t rct2RideType);
    uint8_t OpenRCT2RideTypeToRCT2RideType(ObjectEntryIndex openrct2Type);
    track_type_t RCT2TrackTypeToOpenRCT2(RCT12TrackType origTrackType, uint8_t rideType, bool convertFlat);
    RCT12TrackType OpenRCT2TrackTypeToRCT2(track_type_t origTrackType);

    /**
     * Iterates an RCT2 string buffer and returns the length of the string in bytes.
     * Handles single and multi-byte strings.
     */
    size_t GetRCT2StringBufferLen(const char* buffer, size_t maxBufferLen);

    struct FootpathMapping
    {
        std::string_view Original;
        std::string_view NormalSurface;
        std::string_view QueueSurface;
        std::string_view Railing;
    };

    const FootpathMapping* GetFootpathSurfaceId(
        const ObjectEntryDescriptor& desc, bool ideallyLoaded = false, bool isQueue = false);
    std::optional<rct_object_entry> GetBestObjectEntryForSurface(std::string_view surface, std::string_view railings);

    static constexpr std::string_view DefaultTerrainSurfaces[] = {
        "rct2.terrain_surface.grass",        "rct2.terrain_surface.sand",        "rct2.terrain_surface.dirt",
        "rct2.terrain_surface.rock",         "rct2.terrain_surface.martian",     "rct2.terrain_surface.chequerboard",
        "rct2.terrain_surface.grass_clumps", "rct2.terrain_surface.ice",         "rct2.terrain_surface.grid_red",
        "rct2.terrain_surface.grid_yellow",  "rct2.terrain_surface.grid_purple", "rct2.terrain_surface.grid_green",
        "rct2.terrain_surface.sand_red",     "rct2.terrain_surface.sand_brown",
    };

    // Additional surface styles added to OpenRCT2 as a feature if RCT1 linked
    static constexpr std::string_view OpenRCT2HybridTerrainSurfaces[] = {
        "rct1aa.terrain_surface.roof_red",
        "rct1ll.terrain_surface.roof_grey",
        "rct1ll.terrain_surface.rust",
        "rct1ll.terrain_surface.wood",
    };

    static constexpr std::string_view DefaultTerrainEdges[] = {
        "rct2.terrain_edge.rock",
        "rct2.terrain_edge.wood_red",
        "rct2.terrain_edge.wood_black",
        "rct2.terrain_edge.ice",
    };

    // Additional surface edges added to OpenRCT2 as a feature if RCT1 was linked
    static constexpr std::string_view OpenRCT2HybridTerrainEdges[] = {
        "rct1.terrain_edge.brick",          "rct1.terrain_edge.iron",           "rct1aa.terrain_edge.grey",
        "rct1aa.terrain_edge.yellow",       "rct1aa.terrain_edge.red",          "rct1ll.terrain_edge.purple",
        "rct1ll.terrain_edge.green",        "rct1ll.terrain_edge.stone_brown",  "rct1ll.terrain_edge.stone_grey",
        "rct1ll.terrain_edge.skyscraper_a", "rct1ll.terrain_edge.skyscraper_b",
    };
} // namespace RCT2

std::vector<uint8_t> DecryptSea(const fs::path& path);
