/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../rct12/RCT12.h"
#include "../ride/RideRatings.h"
#include "../ride/VehicleColour.h"
#include "Limits.h"

namespace RCT1
{
    constexpr const uint8_t RCT1_RESEARCH_FLAGS_SEPARATOR = 0xFF;

    enum class RideType : uint8_t
    {
        WoodenRollerCoaster = 0,
        StandUpSteelRollerCoaster,
        SuspendedRollerCoaster,
        InvertedRollerCoaster,
        SteelMiniRollerCoaster,
        MiniatureRailway,
        Monorail,
        SuspendedSingleRailRollerCoaster,
        BoatHire,
        WoodenCrazyRodentRollerCoaster,
        SingleRailRollerCoaster,
        CarRide,
        LaunchedFreefall,
        BobsledRollerCoaster,
        ObservationTower,
        SteelRollerCoaster,
        WaterSlide,
        MineTrainRollerCoaster,
        Chairlift,
        SteelCorkscrewRollerCoaster,
        HedgeMaze,
        SpiralSlide,
        GoKarts,
        LogFlume,
        RiverRapids,
        Dodgems,
        SwingingShip,
        SwingingInverterShip,
        IceCreamStall,
        ChipsStall,
        DrinkStall,
        CandyflossStall,
        BurgerBar,
        MerryGoRound,
        BalloonStall,
        InformationKiosk,
        Toilets,
        FerrisWheel,
        MotionSimulator,
        _3DCinema,
        TopSpin,
        SpaceRings,
        ReverseFreefallRollerCoaster,
        SouvenirStall,
        VerticalRollerCoaster,
        PizzaStall,
        Twist,
        HauntedHouse,
        PopcornStall,
        Circus,
        GhostTrain,
        SteelTwisterRollerCoaster,
        WoodenTwisterRollerCoaster,
        WoodenSideFrictionRollerCoaster,
        SteelWildMouseRollerCoaster,
        HotDogStall,
        ExoticSeaFoodStall,
        HatStall,
        ToffeeAppleStall,
        VirginiaReel,
        RiverRide,
        CycleMonorail,
        FlyingRollerCoaster,
        SuspendedMonorail,
        _40,
        WoodenReverseRollerCoaster,
        HeartlineTwisterRollerCoaster,
        MiniatureGolf,
        _44,
        RotoDrop,
        FlyingSaucers,
        CrookedHouse,
        CycleRailway,
        SuspendedLoopingRollerCoaster,
        WaterCoaster,
        AirPoweredVerticalCoaster,
        InvertedWildMouseCoaster,
        JetSkis,
        TShirtStall,
        RaftRide,
        DoughnutShop,
        Enterprise,
        CoffeeShop,
        FriedChickenStall,
        LemonadeStall,

        Count,
        Null = 255,
    };

#pragma pack(push, 1)
    struct Entrance
    {
        uint16_t x;
        uint16_t y;
        uint16_t z;
        uint8_t direction;
    };
    assert_struct_size(Entrance, 7);

    /**
     * RCT1 ride structure
     * size: 0x260
     */
    struct Ride
    {
        RideType type;            // 0x000
        uint8_t vehicle_type;     // 0x001
        uint16_t lifecycle_flags; // 0x002
        uint8_t operating_mode;   // 0x004
        uint8_t colour_scheme;    // 0x005
        struct
        {
            colour_t body;
            colour_t trim;
        } vehicle_colours[Limits::MaxTrainsPerRide];             // 0x006
        colour_t track_primary_colour;                           // 0x01E
        colour_t track_secondary_colour;                         // 0x01F
        colour_t track_support_colour;                           // 0x020
        uint8_t status;                                          // 0x021
        uint16_t name;                                           // 0x022
        uint16_t name_argument_ride;                             // 0x024
        uint16_t name_argument_number;                           // 0x026
        RCT12xy8 overall_view;                                   // 0x028
        RCT12xy8 station_starts[Limits::MaxStationsPerRide];     // 0x02A
        uint8_t station_height[Limits::MaxStationsPerRide];      // 0x032
        uint8_t station_length[Limits::MaxStationsPerRide];      // 0x036
        uint8_t station_light[Limits::MaxStationsPerRide];       // 0x03A
        uint8_t station_depart[Limits::MaxStationsPerRide];      // 0x03E
        RCT12xy8 entrance[Limits::MaxStationsPerRide];           // 0x042
        RCT12xy8 exit[Limits::MaxStationsPerRide];               // 0x04A
        uint16_t last_peep_in_queue[Limits::MaxStationsPerRide]; // 0x052
        uint8_t num_peeps_in_queue[Limits::MaxStationsPerRide];  // 0x05A
        uint16_t vehicles[Limits::MaxTrainsPerRide];             // 0x05E
        uint8_t depart_flags;                                    // 0x076
        uint8_t num_stations;                                    // 0x077
        uint8_t num_trains;                                      // 0x078
        uint8_t num_cars_per_train;                              // 0x079
        uint8_t proposed_num_vehicles;                           // 0x07A
        uint8_t proposed_num_cars_per_train;                     // 0x07B
        uint8_t max_trains;                                      // 0x07C
        uint8_t min_max_cars_per_train;                          // 0x07D
        uint8_t min_waiting_time;                                // 0x07E
        uint8_t max_waiting_time;                                // 0x07F
        uint8_t operation_option;                                // 0x080
        uint8_t boat_hire_return_direction;                      // 0x081
        RCT12xy8 boat_hire_return_position;                      // 0x082
        uint8_t data_logging_index;                              // 0x084
        uint8_t special_track_elements;                          // 0x085
        uint16_t unk_86;                                         // 0x086
        int32_t max_speed;                                       // 0x088
        int32_t average_speed;                                   // 0x08C
        uint8_t current_test_segment;                            // 0x090
        uint8_t average_speed_test_timeout;                      // 0x091
        uint8_t pad_0E2[0x2];                                    // 0x092
        int32_t length[Limits::MaxStationsPerRide];              // 0x094
        uint16_t time[Limits::MaxStationsPerRide];               // 0x0A4
        fixed16_2dp max_positive_vertical_g;                     // 0x0AC
        fixed16_2dp max_negative_vertical_g;                     // 0x0AE
        fixed16_2dp max_lateral_g;                               // 0x0B0
        fixed16_2dp previous_vertical_g;                         // 0x0B2
        fixed16_2dp previous_lateral_g;                          // 0x0B4
        uint8_t pad_B6[0x2];                                     // 0x0B6
        uint32_t testing_flags;                                  // 0x0B8
        // x y map location of the current track piece during a test
        // this is to prevent counting special tracks multiple times
        RCT12xy8 cur_test_track_location; // 0x0BC
        // Next 3 variables are related (XXXX XYYY ZZZa aaaa)
        uint16_t turn_count_default; // 0x0BE X = current turn count
        uint16_t turn_count_banked;  // 0x0C0
        uint16_t turn_count_sloped;  // 0x0C2 X = number turns > 3 elements
        union
        {
            uint8_t num_inversions; // 0x0C4
            uint8_t num_holes;
        };
        uint8_t num_drops;              // 0x0C5
        uint8_t start_drop_height;      // 0x0C6
        uint8_t highest_drop_height;    // 0x0C7
        int32_t sheltered_length;       // 0x0C8
        uint8_t unk_CC[2];              // 0x0CC
        uint8_t num_sheltered_sections; // 0x0CE
        // see cur_test_track_location
        uint8_t cur_test_track_z; // 0x0CF
        int16_t unk_D0;           // 0x0D0
        int16_t unk_D2;           // 0x0D2
        // Customer count in the last 10 * 960 game ticks (sliding window)
        uint16_t num_customers[Limits::CustomerHistorySize]; // 0xD4
        money16 price;                                       // 0x0E8
        RCT12xy8 chairlift_bullwheel_location[2];            // 0x0EA
        uint8_t chairlift_bullwheel_z[2];                    // 0x0EE
        union
        {
            RatingTuple ratings;
            struct
            {
                ride_rating excitement; // 0x0F0
                ride_rating intensity;  // 0x0F2
                ride_rating nausea;     // 0x0F4
            };
        };
        uint16_t value;                        // 0x0F6
        uint16_t chairlift_bullwheel_rotation; // 0x0F8
        uint8_t satisfaction;                  // 0x0FA
        uint8_t satisfaction_time_out;         // 0x0FB
        uint8_t satisfaction_next;             // 0x0FC
        uint8_t window_invalidate_flags;       // 0x0FD
        uint8_t unk_FE[2];                     // 0x0FE
        uint32_t total_customers;              // 0x100
        money32 total_profit;                  // 0x104
        uint8_t popularity;                    // 0x108
        uint8_t popularity_time_out;           // 0x109
        uint8_t popularity_next;               // 0x10A
        uint8_t num_riders;                    // 0x10B
        uint8_t music_tune_id;                 // 0x10C
        uint8_t slide_in_use;                  // 0x10D
        union
        {
            uint16_t slide_peep; // 0x10E
            uint16_t maze_tiles; // 0x10E
        };
        uint8_t pad_110[0xE];              // 0x110
        uint8_t slide_peep_t_shirt_colour; // 0x11E
        uint8_t pad_11F[0x7];              // 0x11F
        uint8_t spiral_slide_progress;     // 0x126
        uint8_t pad_127[0x9];              // 0x127
        int16_t build_date;                // 0x130
        money16 upkeep_cost;               // 0x131
        uint16_t race_winner;              // 0x132
        uint8_t unk_134[2];                // 0x134
        uint32_t music_position;           // 0x138
        uint8_t breakdown_reason_pending;  // 0x13C
        uint8_t mechanic_status;           // 0x13D
        uint16_t mechanic;                 // 0x13E
        uint8_t inspection_station;        // 0x140
        uint8_t broken_vehicle;            // 0x141
        uint8_t broken_car;                // 0x142
        uint8_t breakdown_reason;          // 0x143
        money16 price_secondary;           // 0x144
        union
        {
            struct
            {
                uint8_t reliability_subvalue;   // 0x146, 0 - 255, acts like the decimals for reliability_percentage
                uint8_t reliability_percentage; // 0x147, Starts at 100 and decreases from there.
            };
            uint16_t reliability; // 0x146
        };
        uint8_t unreliability_factor;                   // 0x148
        uint8_t downtime;                               // 0x149
        uint8_t inspection_interval;                    // 0x14A
        uint8_t last_inspection;                        // 0x14B
        uint8_t unk_14C[20];                            // 0x14C
        money32 income_per_hour;                        // 0x160
        money32 profit;                                 // 0x164
        uint8_t queue_time[Limits::MaxStationsPerRide]; // 0x168
        colour_t track_colour_main[4];                  // 0x16C
        colour_t track_colour_additional[4];            // 0x170
        colour_t track_colour_supports[4];              // 0x174
        uint8_t music;                                  // 0x178
        uint8_t entrance_style;                         // 0x179
        uint8_t unk_17A[230];                           // 0x17A
    };
    assert_struct_size(Ride, 0x260);

    struct UnkEntity : RCT12SpriteBase
    {
        uint8_t pad_1F[3];             // 0x1f
        rct_string_id name_string_idx; // 0x22
        uint16_t var_24;
        uint16_t frame; // 0x26
        uint8_t var_28[3];
        uint8_t var_2B;
        uint8_t pad_2C[0x45];
        uint8_t var_71;
    };

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
        rct_vehicle_colour colours; // 0x32
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

        // The previous vehicle on the same train or the last vehicle on the previous or only train.
        uint16_t prev_vehicle_on_ride; // 0x40

        // The next vehicle on the same train or the first vehicle on the next or only train
        uint16_t next_vehicle_on_ride; // 0x42

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
        int16_t SwingSpeed;
        uint8_t status;                  // 0x50
        uint8_t sub_state;               // 0x51
        uint16_t peep[32];               // 0x52
        uint8_t peep_tshirt_colours[32]; // 0x92
        uint8_t num_seats;               // 0xB2
        uint8_t num_peeps;               // 0xB3
        uint8_t next_free_seat;          // 0xB4
        uint8_t restraints_position;     // 0xB5 0 == Close, 255 == Open
        int16_t spin_speed;
        uint16_t sound2_flags;
        uint8_t spin_sprite;
        uint8_t sound1_id;     // 0xBB
        uint8_t sound1_volume; // 0xBC
        uint8_t sound2_id;     // 0xBD
        uint8_t sound2_volume; // 0xBE
        int8_t sound_vector_factor;
        union
        {
            uint16_t var_C0;
            uint16_t time_waiting;      // 0xC0
            uint16_t cable_lift_target; // 0xC0
        };
        uint8_t speed;                // 0xC2
        uint8_t powered_acceleration; // 0xC3
        union
        {
            uint8_t DodgemsCollisionDetection; // 0xC4
            uint8_t CollisionDetectionTimer;   // 0xC4
        };
        uint8_t animation_frame;
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
        uint8_t mini_golf_flags;  // 0xD5
        uint8_t ride_subtype;     // 0xD6
        uint8_t colours_extended; // 0xD7

        uint16_t GetTrackType() const
        {
            return TrackTypeAndDirection >> 2;
        }

        uint8_t GetTrackDirection() const
        {
            return TrackTypeAndDirection & RCT12VehicleTrackDirectionMask;
        }
    };

    struct Peep : RCT12SpriteBase
    {
        uint8_t pad_1F[3];
        rct_string_id name_string_idx; // 0x22
        uint16_t next_x;               // 0x24
        uint16_t next_y;               // 0x26
        uint8_t next_z;                // 0x28
        uint8_t next_flags;            // 0x29
        uint8_t outside_of_park;       // 0x2A
        uint8_t state;                 // 0x2B
        uint8_t sub_state;             // 0x2C
        uint8_t sprite_type;           // 0x2D
        uint8_t type;                  // 0x2E
        union
        {
            uint8_t staff_type;  // 0x2F
            uint8_t no_of_rides; // 0x2F
        };
        uint8_t tshirt_colour;         // 0x30
        uint8_t trousers_colour;       // 0x31
        uint16_t destination_x;        // 0x32 Location that the peep is trying to get to
        uint16_t destination_y;        // 0x34
        uint8_t destination_tolerance; // 0x36 How close to destination before next action/state 0 = exact
        uint8_t var_37;
        uint8_t energy;           // 0x38
        uint8_t energy_target;    // 0x39
        uint8_t happiness;        // 0x3A
        uint8_t happiness_target; // 0x3B
        uint8_t nausea;           // 0x3C
        uint8_t nausea_target;    // 0x3D
        uint8_t hunger;           // 0x3E
        uint8_t thirst;           // 0x3F
        uint8_t toilet;           // 0x40
        uint8_t mass;             // 0x41
        uint8_t time_to_consume;
        uint8_t intensity; // 0x43 The max intensity is stored in the first 4 bits, and the min intensity in the second 4 bits
        uint8_t nausea_tolerance;        // 0x44
        uint8_t window_invalidate_flags; // 0x45
        money16 paid_on_drink;           // 0x46
        uint8_t ride_types_been_on[16];  // 0x48
        uint8_t pad_5F[0x10];            // 0x58
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
        // Normally 0, 1 for carrying sliding board on spiral slide ride, 2 for carrying lawn mower
        uint8_t special_sprite;     // 0x6D
        uint8_t action_sprite_type; // 0x6E
        // Seems to be used like a local variable, as it's always set before calling SwitchNextActionSpriteType, which reads
        // this again
        uint8_t next_action_sprite_type;    // 0x6F
        uint8_t action_sprite_image_offset; // 0x70
        uint8_t action;                     // 0x71
        uint8_t action_frame;               // 0x72
        uint8_t step_progress;
        union
        {
            uint16_t mechanic_time_since_call; // time getting to ride to fix
            uint16_t next_in_queue;            // 0x74
        };
        uint8_t pad_76;
        uint8_t pad_77;
        union
        {
            uint8_t maze_last_edge; // 0x78
            uint8_t direction;      // Direction ?
        };
        RCT12RideId interaction_ride_index;
        uint16_t time_in_queue;    // 0x7A
        uint8_t rides_been_on[32]; // 0x7C
        // 255 bit bitmap of every ride the peep has been on see
        // window_peep_rides_update for how to use.
        uint32_t id;                                        // 0x9C
        money32 cash_in_pocket;                             // 0xA0
        money32 cash_spent;                                 // 0xA4
        int32_t park_entry_time;                            // 0xA8
        int8_t rejoin_queue_timeout;                        // 0xAC
        RCT12RideId previous_ride;                          // 0xAD
        uint16_t previous_ride_time_out;                    // 0xAE
        RCT12PeepThought thoughts[Limits::MaxPeepThoughts]; // 0xB0
        uint8_t pad_C4;
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
        // 0x3F Litter Count split into lots of 3 with time, 0xC0 Time since last recalc
        uint8_t litter_count; // 0xE1
        union
        {
            uint8_t time_on_ride;         // 0xE2
            uint8_t staff_mowing_timeout; // 0xE2
        };
        // 0x3F Sick Count split into lots of 3 with time, 0xC0 Time since last recalc
        uint8_t disgusting_count;             // 0xE3
        money16 paid_to_enter;                // 0xE4
        money16 paid_on_rides;                // 0xE6
        money16 paid_on_food;                 // 0xE8
        money16 paid_on_souvenirs;            // 0xEA
        uint8_t no_of_food;                   // 0xEC
        uint8_t no_of_drinks;                 // 0xED
        uint8_t no_of_souvenirs;              // 0xEE
        uint8_t vandalism_seen;               // 0xEF
        uint8_t voucher_type;                 // 0xF0
        RCT12RideId voucher_arguments;        // 0xF1 ride_id or string_offset_id
        uint8_t surroundings_thought_timeout; // 0xF2
        uint8_t angriness;                    // 0xF3
        uint8_t time_lost;
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
            return item_standard_flags;
        }
    };
    assert_struct_size(Peep, 0x100);

    enum RCT1_PEEP_SPRITE_TYPE
    {
        RCT1_PEEP_SPRITE_TYPE_NORMAL = 0,
        RCT1_PEEP_SPRITE_TYPE_HANDYMAN = 1,
        RCT1_PEEP_SPRITE_TYPE_MECHANIC = 2,
        RCT1_PEEP_SPRITE_TYPE_SECURITY = 3,
        RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_PANDA = 4,
        RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_TIGER = 5,
        RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_ELEPHANT = 6,
        RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_ROMAN = 7,
        RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_GORILLA = 8,
        RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_SNOWMAN = 9,
        RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_KNIGHT = 10,
        RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_ASTRONAUT = 11,

        RCT1_PEEP_SPRITE_TYPE_BALLOON = 16,
        RCT1_PEEP_SPRITE_TYPE_CANDYFLOSS = 17,
        RCT1_PEEP_SPRITE_TYPE_UMBRELLA = 18,
        RCT1_PEEP_SPRITE_TYPE_PIZZA = 19,        // Unsure
        RCT1_PEEP_SPRITE_TYPE_SECURITY_ALT = 20, // Unknown
        RCT1_PEEP_SPRITE_TYPE_POPCORN = 21,
        RCT1_PEEP_SPRITE_TYPE_ARMS_CROSSED = 22,
        RCT1_PEEP_SPRITE_TYPE_HEAD_DOWN = 23,
        RCT1_PEEP_SPRITE_TYPE_NAUSEOUS = 24,
        RCT1_PEEP_SPRITE_TYPE_VERY_NAUSEOUS = 25,
        RCT1_PEEP_SPRITE_TYPE_REQUIRE_TOILET = 26,
        RCT1_PEEP_SPRITE_TYPE_HAT = 27,
        RCT1_PEEP_SPRITE_TYPE_BURGER = 28,
        RCT1_PEEP_SPRITE_TYPE_TENTACLE = 29,
        RCT1_PEEP_SPRITE_TYPE_TOFFEE_APPLE = 30
    };

    union Entity
    {
        uint8_t pad_00[0x100];
        UnkEntity unknown;
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
    };
    assert_struct_size(Entity, 0x100);

    struct ResearchItem
    {
        uint8_t item;
        uint8_t related_ride;
        uint8_t type;
        uint8_t flags;
        uint8_t category;
    };
    assert_struct_size(ResearchItem, 5);

    /**
     * RCT1,AA,LL scenario / saved game structure.
     * size: 0x1F850C
     */
    struct S4
    {
        uint16_t month;
        uint16_t day;
        uint32_t ticks;
        uint32_t random_a;
        uint32_t random_b;
        RCT12TileElement tile_elements[Limits::MaxTileElements];
        uint32_t unk_counter;
        Entity sprites[Limits::MaxEntities];
        uint16_t next_sprite_index;
        uint16_t first_vehicle_sprite_index;
        uint16_t first_peep_sprite_index;
        uint16_t first_duck_sprite_index;
        uint16_t first_litter_sprite_index;
        uint16_t first_oversized_ride_car_sprite_index;
        uint16_t sprites_available;
        uint16_t num_vehicle_sprites;
        uint16_t num_peep_sprites;
        uint16_t num_duck_sprites;
        uint16_t num_litter_sprites;
        uint16_t num_oversized_ride_car_sprites;
        uint32_t park_name_string_index;
        uint32_t unk_198830;
        money32 cash;
        money32 loan;
        uint32_t park_flags;
        money16 park_entrance_fee;
        Entrance park_entrance;
        uint8_t unk_198849;
        rct12_peep_spawn peep_spawn[Limits::MaxPeepSpawns];
        uint8_t unk_198856;
        uint8_t research_level;
        uint32_t unk_198858;
        uint32_t available_rides[8];
        uint32_t available_vehicles[8];
        uint32_t ride_feature_1[128];
        uint32_t ride_feature_2[128];
        uint16_t guests_in_park;
        uint16_t unk_198C9E;
        money32 expenditure[Limits::ExpenditureTableMonthCount][Limits::ExpenditureTypeCount];
        uint32_t guests_in_park_2;
        uint8_t unk_199024;
        colour_t handman_colour;
        colour_t mechanic_colour;
        colour_t security_guard_colour;
        uint8_t available_scenery[128];
        uint16_t available_banners;
        uint8_t unk_1990AA[94];
        uint16_t park_rating;
        uint8_t park_rating_history[32];
        uint8_t guests_in_park_history[32];
        uint8_t research_priority;
        uint8_t research_progress_stage;
        uint8_t last_research_item;
        uint8_t last_research_ride;
        uint8_t last_research_type;
        uint8_t last_research_flags;
        ResearchItem research_items[200];
        uint8_t next_research_item;
        uint8_t next_research_ride;
        uint8_t next_research_type;
        uint8_t next_research_flags;
        uint16_t research_progress;
        uint8_t next_research_category;
        uint8_t next_research_expected_day;
        uint8_t next_research_expected_month;
        uint8_t guest_initial_happiness;
        uint16_t park_size;
        uint16_t guest_generation_probability;
        money16 total_ride_value_for_money;
        money32 max_loan;
        money16 guest_initial_cash;
        uint8_t guest_initial_hunger;
        uint8_t guest_initial_thirst;
        uint8_t scenario_objective_type;
        uint8_t scenario_objective_years;
        uint16_t unk_199552;
        money32 scenario_objective_currency;
        uint16_t scenario_objective_num_guests;
        uint8_t marketing_status[20];
        uint8_t marketing_assoc[20];
        uint8_t unk_199582[2];
        money32 cash_history[Limits::FinanceGraphSize];
        money32 total_expenditure;
        money32 profit;
        uint8_t unk_199788[8];
        money32 weekly_profit_history[Limits::FinanceGraphSize];
        money32 park_value;
        money32 park_value_history[Limits::FinanceGraphSize];
        money32 completed_company_value;
        uint32_t num_admissions;
        money32 admission_total_income;
        money32 company_value;
        uint8_t thought_timer[16];
        rct12_award awards[Limits::MaxAwards];
        money16 land_price;
        money16 construction_rights_price;
        uint16_t unk_199BCC;
        uint16_t unk_199BCE;
        uint32_t unk_199BD0;
        char username[64];
        uint32_t game_version;
        money32 objective_completion_company_value;
        uint32_t finance_checksum;
        uint16_t num_rides;
        uint16_t cheat_detection_neg_num_rides;
        uint16_t cheat_detection_max_owned_tiles;
        uint16_t cheat_detection_neg_max_owned_tiles;
        uint32_t finance_checksum_3;
        uint32_t scenario_slot_index_checksum;
        char scenario_winner[32];
        uint32_t finance_checksum_2;
        char copyright_notice[40];
        uint16_t cheat_detection_sv6_sc4[4];
        uint16_t unk_199C84;
        uint16_t unk_199C86;
        uint16_t map_size_units;
        uint16_t map_size_unk_b;
        uint16_t map_size;
        uint16_t map_size_max_xy;
        uint32_t same_price_throughout;
        uint16_t unk_199C94;
        uint8_t unk_199C96[3];
        uint8_t water_colour;
        uint16_t unk_199C9A;
        ResearchItem research_items_LL[180];
        uint8_t unk_19A020[5468];
        RCT12Banner banners[Limits::MaxBanners];
        char string_table[Limits::MaxUserStrings][Limits::MaxUserStringLength];
        uint32_t game_time_counter;
        Ride rides[Limits::MaxRidesInPark];
        uint16_t unk_game_time_counter;
        int16_t view_x;
        int16_t view_y;
        uint8_t view_zoom;
        uint8_t view_rotation;
        RCT12MapAnimation map_animations[Limits::MaxAnimatedObjects];
        uint32_t num_map_animations;
        uint8_t unk_1CADBC[12];
        uint16_t scrolling_text_step;
        uint32_t unk_1CADCA;
        uint16_t unk_1CADCE;
        uint8_t unk_1CADD0[116];
        RCT12RideMeasurement ride_measurements[8];
        uint32_t next_guest_index;
        uint16_t game_counter_5;
        uint8_t patrol_areas[(Limits::MaxStaff + Limits::StaffTypeCount) * Limits::PatrolAreaSize];
        uint8_t staff_modes[Limits::MaxStaff];
        uint8_t unk_1F431E[4];
        uint8_t unk_1F4322[8];
        uint8_t climate;
        uint8_t unk_1F432B;
        uint16_t climate_timer;
        uint8_t weather;
        uint8_t target_weather;
        uint8_t temperature;
        uint8_t target_temperature;
        uint8_t thunder_frequency;
        uint8_t target_thunder_frequency;
        uint8_t weather_gloom;
        uint8_t target_weather_gloom;
        uint8_t rain;
        uint8_t target_rain;
        rct12_news_item messages[Limits::MaxNewsItems];
        char scenario_name[62];
        uint16_t scenario_slot_index;
        uint32_t scenario_flags;
        uint8_t unk_1F8358[432];
        uint32_t expansion_pack_checksum;
    };
    assert_struct_size(S4, 0x1F850C);

    /**
     * Track design structure. Only for base RCT1
     * size: 0x2006
     */
    struct TD4
    {
        RideType type; // 0x00
        uint8_t vehicle_type;
        uint32_t flags;                                               // 0x02
        uint8_t mode;                                                 // 0x06
        uint8_t version_and_colour_scheme;                            // 0x07 0b0000_VVCC
        rct_vehicle_colour vehicle_colours[Limits::MaxTrainsPerRide]; // 0x08
        uint8_t track_spine_colour_v0;                                // 0x20
        uint8_t track_rail_colour_v0;                                 // 0x21
        uint8_t track_support_colour_v0;                              // 0x22
        uint8_t depart_flags;                                         // 0x23
        uint8_t number_of_trains;                                     // 0x24
        uint8_t number_of_cars_per_train;                             // 0x25
        uint8_t min_waiting_time;                                     // 0x26
        uint8_t max_waiting_time;                                     // 0x27
        union
        {
            uint8_t operation_setting;
            uint8_t launch_speed;
            uint8_t NumLaps;
            uint8_t max_people;
        };
        int8_t max_speed;                // 0x29
        int8_t average_speed;            // 0x2A
        uint16_t ride_length;            // 0x2B
        uint8_t max_positive_vertical_g; // 0x2D
        int8_t max_negative_vertical_g;  // 0x2C
        uint8_t max_lateral_g;           // 0x2F
        union
        {
            uint8_t num_inversions; // 0x30
            uint8_t num_holes;      // 0x30
        };
        uint8_t num_drops;           // 0x31
        uint8_t highest_drop_height; // 0x32
        uint8_t excitement;          // 0x33
        uint8_t intensity;           // 0x34
        uint8_t nausea;              // 0x35
        money16 upkeep_cost;         // 0x36
    };

    assert_struct_size(TD4, 0x38);

    /**
     * Track design structure for Added Attractions / Loopy Landscapes
     * size: 0x2006
     */
    struct TD4AA : public TD4
    {
        uint8_t track_spine_colour[Limits::NumColourSchemes];   // 0x38
        uint8_t track_rail_colour[Limits::NumColourSchemes];    // 0x3C
        uint8_t track_support_colour[Limits::NumColourSchemes]; // 0x40
        uint8_t flags2;                                         // 0x44

        uint8_t pad_45[0x7F]; // 0x45
    };

    assert_struct_size(TD4AA, 0xC4);
#pragma pack(pop)

    enum
    {
        RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN = 0,
        RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN_BACKWARDS,
        RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_INVERTED_COASTER_TRAIN, // Not in RCT2
        RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_CARS,
        RCT1_VEHICLE_TYPE_LADYBIRD_CARS,
        RCT1_VEHICLE_TYPE_STANDUP_ROLLER_COASTER_CARS,
        RCT1_VEHICLE_TYPE_SPINNING_CARS,
        RCT1_VEHICLE_TYPE_SINGLE_PERSON_SWINGING_CHAIRS,
        RCT1_VEHICLE_TYPE_SWANS_PEDAL_BOATS,
        RCT1_VEHICLE_TYPE_LARGE_MONORAIL_TRAIN,
        RCT1_VEHICLE_TYPE_CANOES,
        RCT1_VEHICLE_TYPE_ROWING_BOATS,
        RCT1_VEHICLE_TYPE_STEAM_TRAIN,
        RCT1_VEHICLE_TYPE_WOODEN_MOUSE_CARS,
        RCT1_VEHICLE_TYPE_BUMPER_BOATS,
        RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN_BACKWARDS,
        RCT1_VEHICLE_TYPE_ROCKET_CARS,
        RCT1_VEHICLE_TYPE_HORSES, // Steeplechase
        RCT1_VEHICLE_TYPE_SPORTSCARS,
        RCT1_VEHICLE_TYPE_LYING_DOWN_SWINGING_CARS, // Inverted single-rail
        RCT1_VEHICLE_TYPE_WOODEN_MINE_CARS,
        RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_AIRPLANE_CARS,
        RCT1_VEHICLE_TYPE_SMALL_MONORAIL_CARS,
        RCT1_VEHICLE_TYPE_WATER_TRICYCLES,
        RCT1_VEHICLE_TYPE_LAUNCHED_FREEFALL_CAR,
        RCT1_VEHICLE_TYPE_BOBSLEIGH_CARS,
        RCT1_VEHICLE_TYPE_DINGHIES,
        RCT1_VEHICLE_TYPE_ROTATING_CABIN,
        RCT1_VEHICLE_TYPE_MINE_TRAIN,
        RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS,
        RCT1_VEHICLE_TYPE_CORKSCREW_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_MOTORBIKES,
        RCT1_VEHICLE_TYPE_RACING_CARS,
        RCT1_VEHICLE_TYPE_TRUCKS,
        RCT1_VEHICLE_TYPE_GO_KARTS,
        RCT1_VEHICLE_TYPE_RAPIDS_BOATS,
        RCT1_VEHICLE_TYPE_LOG_FLUME_BOATS,
        RCT1_VEHICLE_TYPE_DODGEMS,
        RCT1_VEHICLE_TYPE_SWINGING_SHIP,
        RCT1_VEHICLE_TYPE_SWINGING_INVERTER_SHIP,
        RCT1_VEHICLE_TYPE_MERRY_GO_ROUND,
        RCT1_VEHICLE_TYPE_FERRIS_WHEEL,
        RCT1_VEHICLE_TYPE_SIMULATOR_POD,
        RCT1_VEHICLE_TYPE_CINEMA_BUILDING,
        RCT1_VEHICLE_TYPE_TOPSPIN_CAR,
        RCT1_VEHICLE_TYPE_SPACE_RINGS,
        RCT1_VEHICLE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER_CAR,
        RCT1_VEHICLE_TYPE_VERTICAL_ROLLER_COASTER_CARS,
        RCT1_VEHICLE_TYPE_CAT_CARS,
        RCT1_VEHICLE_TYPE_TWIST_ARMS_AND_CARS,
        RCT1_VEHICLE_TYPE_HAUNTED_HOUSE_BUILDING,
        RCT1_VEHICLE_TYPE_LOG_CARS,
        RCT1_VEHICLE_TYPE_CIRCUS_TENT,
        RCT1_VEHICLE_TYPE_GHOST_TRAIN_CARS,
        RCT1_VEHICLE_TYPE_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_WOODEN_TWISTER_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_WOODEN_SIDE_FRICTION_CARS,
        RCT1_VEHICLE_TYPE_VINTAGE_CARS,
        RCT1_VEHICLE_TYPE_STEAM_TRAIN_COVERED_CARS,
        RCT1_VEHICLE_TYPE_STAND_UP_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_FLOORLESS_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_STEEL_MOUSE_CARS,
        RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS_ALTERNATIVE,
        RCT1_VEHICLE_TYPE_SUSPENDED_MONORAIL_TRAIN,
        RCT1_VEHICLE_TYPE_HELICOPTER_CARS,
        RCT1_VEHICLE_TYPE_VIRGINIA_REEL_TUBS,
        RCT1_VEHICLE_TYPE_REVERSER_CARS,
        RCT1_VEHICLE_TYPE_GOLFERS,
        RCT1_VEHICLE_TYPE_RIVER_RIDE_BOATS,
        RCT1_VEHICLE_TYPE_FLYING_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_NON_LOOPING_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS,
        RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS_REVERSED,
        RCT1_VEHICLE_TYPE_RESERVED,
        RCT1_VEHICLE_TYPE_ROTODROP_CAR,
        RCT1_VEHICLE_TYPE_FLYING_SAUCERS,
        RCT1_VEHICLE_TYPE_CROOKED_HOUSE_BUILDING,
        RCT1_VEHICLE_TYPE_BICYCLES,
        RCT1_VEHICLE_TYPE_HYPERCOASTER_TRAIN,
        RCT1_VEHICLE_TYPE_4_ACROSS_INVERTED_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_WATER_COASTER_BOATS,
        RCT1_VEHICLE_TYPE_FACEOFF_CARS,
        RCT1_VEHICLE_TYPE_JET_SKIS,
        RCT1_VEHICLE_TYPE_RAFT_BOATS,
        RCT1_VEHICLE_TYPE_AMERICAN_STYLE_STEAM_TRAIN,
        RCT1_VEHICLE_TYPE_AIR_POWERED_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_SUSPENDED_WILD_MOUSE_CARS, // Inverted Hairpin in RCT2
        RCT1_VEHICLE_TYPE_ENTERPRISE_WHEEL,

        RCT1_VEHICLE_TYPE_COUNT
    };

    enum
    {
        RCT1_TRACK_ELEM_BOOSTER = 100
    };

    enum
    {
        RCT1_RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE = 2,
        RCT1_RIDE_MODE_POWERED_LAUNCH = 3,
    };

    enum
    {
        RCT1_RIDE_DEPART_PLAY_MUSIC = 1 << 5,
    };

    enum
    {
        RCT1_SCENERY_THEME_GENERAL,
        RCT1_SCENERY_THEME_MINE,
        RCT1_SCENERY_THEME_CLASSICAL_ROMAN,
        RCT1_SCENERY_THEME_EGYPTIAN,
        RCT1_SCENERY_THEME_MARTIAN,
        RCT1_SCENERY_THEME_JUMPING_FOUNTAINS, // Single researchable scenery item
        RCT1_SCENERY_THEME_WONDERLAND,
        RCT1_SCENERY_THEME_JURASSIC,
        RCT1_SCENERY_THEME_SPOOKY,
        RCT1_SCENERY_THEME_JUNGLE,
        RCT1_SCENERY_THEME_ABSTRACT,
        RCT1_SCENERY_THEME_GARDEN_CLOCK, // Single researchable scenery item
        RCT1_SCENERY_THEME_SNOW_ICE,
        RCT1_SCENERY_THEME_MEDIEVAL,
        RCT1_SCENERY_THEME_SPACE,
        RCT1_SCENERY_THEME_CREEPY,
        RCT1_SCENERY_THEME_URBAN,
        RCT1_SCENERY_THEME_PAGODA,
    };

    enum
    {
        RCT1_FOOTPATH_TYPE_QUEUE_BLUE,
        RCT1_FOOTPATH_TYPE_QUEUE_RED,
        RCT1_FOOTPATH_TYPE_QUEUE_YELLOW,
        RCT1_FOOTPATH_TYPE_QUEUE_GREEN,

        RCT1_FOOTPATH_TYPE_TARMAC_GREY,
        RCT1_FOOTPATH_TYPE_TARMAC_RED,
        RCT1_FOOTPATH_TYPE_TARMAC_BROWN,
        RCT1_FOOTPATH_TYPE_TARMAC_GREEN,

        RCT1_FOOTPATH_TYPE_DIRT_BROWN,
        RCT1_FOOTPATH_TYPE_DIRT_BLACK,

        RCT1_FOOTPATH_TYPE_CRAZY_PAVING = 12,

        RCT1_FOOTPATH_TYPE_ROAD = 16,

        RCT1_FOOTPATH_TYPE_TILE_BROWN = 20,
        RCT1_FOOTPATH_TYPE_TILE_GREY,
        RCT1_FOOTPATH_TYPE_TILE_RED,
        RCT1_FOOTPATH_TYPE_TILE_GREEN,
    };

    enum
    {
        FOOTPATH_SUPPORTS_WOODEN_TRUSS,
        FOOTPATH_SUPPORTS_WOOD,
        FOOTPATH_SUPPORTS_STEEL,
        FOOTPATH_SUPPORTS_BAMBOO,
    };

    enum
    {
        RCT1_PATH_ADDITION_NONE,
        RCT1_PATH_ADDITION_LAMP_1,
        RCT1_PATH_ADDITION_LAMP_2,
        RCT1_PATH_ADDITION_BIN,
        RCT1_PATH_ADDITION_BENCH,
        RCT1_PATH_ADDITION_JUMPING_FOUNTAIN,
        RCT1_PATH_ADDITION_LAMP_3,
        RCT1_PATH_ADDITION_LAMP_4,
        RCT1_PATH_ADDITION_BROKEN_LAMP_1,
        RCT1_PATH_ADDITION_BROKEN_LAMP_2,
        RCT1_PATH_ADDITION_BROKEN_BIN,
        RCT1_PATH_ADDITION_BROKEN_BENCH,
        RCT1_PATH_ADDITION_BROKEN_LAMP_3,
        RCT1_PATH_ADDITION_BROKEN_LAMP_4,
        RCT1_PATH_ADDITION_JUMPING_SNOW,
    };

    enum
    {
        RCT1_WALL_TYPE_MESH_FENCE = 0,
        RCT1_WALL_TYPE_MESH_FENCE_WITH_GATE = 1,
        RCT1_WALL_TYPE_ROMAN = 2,
        RCT1_WALL_TYPE_EGYPTIAN = 3,
        RCT1_WALL_TYPE_HEDGE = 4,
        RCT1_WALL_TYPE_HEDGE_WITH_GATE = 5,
        RCT1_WALL_TYPE_BLUE_PLAYING_CARDS = 6,
        RCT1_WALL_TYPE_RED_PLAYING_CARDS = 7,
        RCT1_WALL_TYPE_WHITE_RAILING = 8,
        RCT1_WALL_TYPE_WHITE_RAILING_WITH_GATE = 9,
        RCT1_WALL_TYPE_MARTIAN = 10,
        RCT1_WALL_TYPE_GLASS_SMOOTH = 11,
        RCT1_WALL_TYPE_WOODEN_PANEL_FENCE = 12,
        RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_GATE = 13,
        RCT1_WALL_TYPE_WOODEN_POST_FENCE = 14,
        RCT1_WALL_TYPE_RED_WOODEN_POST_FENCE = 15,
        RCT1_WALL_TYPE_BARBED_WIRE = 16,
        RCT1_WALL_TYPE_BARBED_WIRE_WITH_GATE = 17,
        RCT1_WALL_TYPE_PRIMITIVE_TALL_WOOD_FENCE = 18,
        RCT1_WALL_TYPE_PRIMITIVE_SHORT_WOOD_FENCE = 19,
        RCT1_WALL_TYPE_IRON_RAILING = 20,
        RCT1_WALL_TYPE_IRON_RAILING_WITH_GATE = 21,
        RCT1_WALL_TYPE_GLASS_PANELS = 22,
        RCT1_WALL_TYPE_BONE_FENCE = 23,
        RCT1_WALL_TYPE_BRICK = 24,
        RCT1_WALL_TYPE_BRICK_WITH_GATE = 25,
        RCT1_WALL_TYPE_WHITE_WOODEN_PANEL_FENCE = 26,
        RCT1_WALL_TYPE_RED_WOODEN_PANEL_FENCE = 27,
        RCT1_WALL_TYPE_STONE = 28,
        RCT1_WALL_TYPE_STONE_WITH_GATE = 29,
        RCT1_WALL_TYPE_WOODEN_FENCE = 30,
        RCT1_WALL_TYPE_JUNGLE = 31,
        RCT1_WALL_TYPE_CONIFER_HEDGE = 32,
        RCT1_WALL_TYPE_CONIFER_HEDGE_WITH_GATE = 33,
        RCT1_WALL_TYPE_SMALL_BROWN_CASTLE = 34,
        RCT1_WALL_TYPE_SMALL_GREY_CASTLE = 35,
        RCT1_WALL_TYPE_ROMAN_COLUMN = 36,
        RCT1_WALL_TYPE_LARGE_BROWN_CASTLE = 37,
        RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_CROSS = 38,
        RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_GATE = 39,
        RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_WINDOW = 40,
        RCT1_WALL_TYPE_MEDIUM_BROWN_CASTLE = 41,
        RCT1_WALL_TYPE_LARGE_GREY_CASTLE = 42,
        RCT1_WALL_TYPE_LARGE_GREY_CASTLE_CROSS = 43,
        RCT1_WALL_TYPE_LARGE_GREY_CASTLE_GATE = 44,
        RCT1_WALL_TYPE_LARGE_GREY_CASTLE_WINDOW = 45,
        RCT1_WALL_TYPE_MEDIUM_GREY_CASTLE = 46,
        RCT1_WALL_TYPE_CREEPY = 47,
        RCT1_WALL_TYPE_CREEPY_GATE = 48,
        RCT1_WALL_TYPE_BARBED_WIRE_WITH_SNOW = 49,
        RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_SNOW = 50,
        RCT1_WALL_TYPE_WOODEN_POST_FENCE_WITH_SNOW = 51,
    };

    enum
    {
        RCT1_RESEARCH_END_AVAILABLE = 0xFF,
        RCT1_RESEARCH_END_RESEARCHABLE = 0xFE,
        RCT1_RESEARCH_END = 0xFD,
    };

    enum
    {
        RCT1_RESEARCH_TYPE_THEME,
        RCT1_RESEARCH_TYPE_RIDE,
        RCT1_RESEARCH_TYPE_VEHICLE,
        RCT1_RESEARCH_TYPE_SPECIAL,
    };

    enum
    {
        RCT1_RESEARCH_CATEGORY_ROLLERCOASTERS = 1 << 0,
        RCT1_RESEARCH_CATEGORY_THRILL_RIDES = 1 << 1,
        RCT1_RESEARCH_CATEGORY_GENTLE_TRANSPORT_RIDES = 1 << 2,
        RCT1_RESEARCH_CATEGORY_SHOPS = 1 << 3,
        RCT1_RESEARCH_CATEGORY_SCENERY_THEMING = 1 << 4,
        RCT1_RESEARCH_CATEGORY_RIDE_IMPROVEMENTS = 1 << 5,
    };

    // Unconfirmed special track elements for research
    enum
    {
        RCT1_RESEARCH_SPECIAL_BANKED_CURVES = 0x06,
        RCT1_RESEARCH_SPECIAL_VERTICAL_LOOP = 0x07,
        RCT1_RESEARCH_SPECIAL_STEEP_TWIST = 0x0C,
        RCT1_RESEARCH_SPECIAL_INLINE_TWIST = 0x11,
        RCT1_RESEARCH_SPECIAL_HALF_LOOP = 0x12,
        RCT1_RESEARCH_SPECIAL_CORKSCREW = 0x13,
        RCT1_RESEARCH_SPECIAL_BANKED_HELIX_A = 0x15,
        RCT1_RESEARCH_SPECIAL_BANKED_HELIX_B = 0x16,
        RCT1_RESEARCH_SPECIAL_HELIX = 0x17,
        RCT1_RESEARCH_SPECIAL_ON_RIDE_PHOTO = 0x1A,
        RCT1_RESEARCH_SPECIAL_WATER_SPLASH = 0x1B,
        RCT1_RESEARCH_SPECIAL_VERTICAL_DROP = 0x1C,
        RCT1_RESEARCH_SPECIAL_BARREL_ROLL = 0x1D,
        RCT1_RESEARCH_SPECIAL_LAUNCHED_LIFT_HILL = 0x1E,
        RCT1_RESEARCH_SPECIAL_LARGE_LOOP_AND_HALF = 0x1F,
        RCT1_RESEARCH_SPECIAL_REVERSER_TURNTABLE = 0x21,
        RCT1_RESEARCH_SPECIAL_HEARTLINE_ROLL = 0x22,
        RCT1_RESEARCH_SPECIAL_REVERSING_SECTIONS = 0x23,
    };

    enum
    {
        RCT1_SCENARIO_FLAG_0 = 1 << 0,
        RCT1_SCENARIO_FLAG_1 = 1 << 1,
        RCT1_SCENARIO_FLAG_2 = 1 << 2,
        RCT1_SCENARIO_FLAG_3 = 1 << 3,
        RCT1_SCENARIO_FLAG_ENABLE_BANNERS = 1 << 4,
        RCT1_SCENARIO_FLAG_5 = 1 << 5,
        RCT1_SCENARIO_FLAG_6 = 1 << 6,
        RCT1_SCENARIO_FLAG_7 = 1 << 7,
        RCT1_SCENARIO_FLAG_CUSTOM_PARK_ENTRANCE_PATH = 1 << 8,
        RCT1_SCENARIO_FLAG_NO_CASH_RESET = 1 << 9,
        RCT1_SCENARIO_FLAG_10 = 1 << 10,
        RCT1_SCENARIO_FLAG_11 = 1 << 11,
        RCT1_SCENARIO_FLAG_12 = 1 << 12,
        RCT1_SCENARIO_FLAG_CUSTOM_MAP_SIZE = 1 << 13,
        RCT1_SCENARIO_FLAG_14 = 1 << 14,
        RCT1_SCENARIO_FLAG_15 = 1 << 15,
        RCT1_SCENARIO_FLAG_16 = 1 << 16,
        RCT1_SCENARIO_FLAG_17 = 1 << 17,
        RCT1_SCENARIO_FLAG_18 = 1 << 18,
        RCT1_SCENARIO_FLAG_19 = 1 << 19,
    };

    enum
    {
        RCT1_PARK_FLAGS_PARK_OPEN = (1 << 0),
        RCT1_PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT = (1 << 1),
        RCT1_PARK_FLAGS_FORBID_LANDSCAPE_CHANGES = (1 << 2),
        RCT1_PARK_FLAGS_FORBID_TREE_REMOVAL = (1 << 3),
        RCT1_PARK_FLAGS_SHOW_REAL_GUEST_NAMES = (1 << 4),
        RCT1_PARK_FLAGS_FORBID_HIGH_CONSTRUCTION = (1 << 5), // Below tree height
        RCT1_PARK_FLAGS_PREF_LESS_INTENSE_RIDES = (1 << 6),
        RCT1_PARK_FLAGS_FORBID_MARKETING_CAMPAIGN = (1 << 7),
        RCT1_PARK_FLAGS_ANTI_CHEAT_DEPRECATED = (1 << 8), // Not used anymore, used for cheat detection
        RCT1_PARK_FLAGS_PREF_MORE_INTENSE_RIDES = (1 << 9),
        RCT1_PARK_FLAGS_NO_MONEY = (1 << 11), // Used for both scenarios and saved games, unlike RCT2
        RCT1_PARK_FLAGS_DIFFICULT_GUEST_GENERATION = (1 << 12),
        RCT1_PARK_FLAGS_PARK_ENTRY_LOCKED_AT_FREE = (1 << 13), // Off: rides and park entry chargeable. On: only rides
                                                               // chargeable.
        RCT1_PARK_FLAGS_DIFFICULT_PARK_RATING = (1 << 14),
        RCT1_PARK_FLAGS_LOCK_REAL_NAMES_OPTION_DEPRECATED = (1 << 15), // Deprecated now we use a persistent 'real names'
                                                                       // setting
    };

    enum
    {
        STEEL_RC_FRONT = 0,
        STEEL_RC_CARRIAGE = 1,
        WOODEN_RC_TRAIN = 2,
        MONORAIL_CARRIAGE = 10,
        MONORAIL_FRONT = 11,
        MONORAIL_BACK = 12,
        MINIATURE_RAILWAY_TENDER = 15,
        MINIATURE_RAILWAY_LOCOMOTIVE = 16,
        MINIATURE_RAILWAY_CARRIAGE = 17,
        MINE_TRAIN_FRONT = 35,
        MINE_TRAIN_CARRIAGE = 36,
        CORKSCREW_RC_FRONT = 38,
        CORKSCREW_RC_CARRIAGE = 39,
        GHOST_TRAIN_CAR = 63,
        TWISTER_RC_SPOILER = 64,
        TWISTER_RC_CARRIAGE = 65,
        GHOST_TRAIN_INVISIBLE = 66,
        ARTICULATED_RC_FRONT = 67,
        ARTICULATED_RC_CARRIAGE = 68,
        MINIATURE_RAILWAY_CARRIAGE_COVERED = 71,
        STANDUP_TWISTER_RC_CARRIAGE = 72,
        REVERSER_RC_CAR = 79,
        REVERSER_RC_BOGIE = 80,
        MINIGOLF_PLAYER = 81,
        MINIGOLF_BALL = 82,
        SPLASH_BOAT = 83,
        SPLASH_BOAT_INVISIBLE = 84,
        HYPERCOASTER_FRONT = 96,
        HYPERCOASTER_CARRIAGE = 97,
        INVERTED_4_ACROSS_CARRIAGE = 98,
        WATER_COASTER_BOAT = 99,
        WATER_COASTER_INVISIBLE = 101,
        RIVER_RAFT = 103,
        MINIATURE_RAILWAY_AMERICAN_TENDER = 104,
        MINIATURE_RAILWAY_AMERICAN_LOCOMOTIVE = 105,
    };

    enum
    {
        COPY_COLOUR_1 = -1,
        COPY_COLOUR_2 = -2,
    };

    enum
    {
        RCT1_WATER_CYAN,
        RCT1_WATER_ORANGE
    };

    enum
    {
        RCT1_SCENERY_TULIPS_1 = 65,
        RCT1_SCENERY_TULIPS_2 = 68,

        RCT1_SCENERY_GEOMETRIC_SCULPTURE_1 = 157, // TGE1
        RCT1_SCENERY_GEOMETRIC_SCULPTURE_2 = 162, // TGE2
        RCT1_SCENERY_GEOMETRIC_SCULPTURE_3 = 168, // TGE3
        RCT1_SCENERY_GEOMETRIC_SCULPTURE_4 = 170, // TGE4
        RCT1_SCENERY_GEOMETRIC_SCULPTURE_5 = 171, // TGE5

        RCT1_SCENERY_SMALL_RED_GARDENS = 176, // TG19
    };

    enum
    {
        RCT1_LANDSCAPE_DOOR_CLOSED = 0,
        RCT1_LANDSCAPE_DOOR_HALF_OPEN = 2,
        RCT1_LANDSCAPE_DOOR_OPEN = 3,
    };

    enum
    {
        RCT1_PATH_SUPPORT_TYPE_TRUSS,
        RCT1_PATH_SUPPORT_TYPE_COATED_WOOD,
        RCT1_PATH_SUPPORT_TYPE_SPACE,
        RCT1_PATH_SUPPORT_TYPE_BAMBOO,
    };

    track_type_t RCT1TrackTypeToOpenRCT2(RCT12TrackType origTrackType, uint8_t rideType);
} // namespace RCT1

void load_from_sv4(const char* path);
void load_from_sc4(const char* path);
