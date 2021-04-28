/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Entity.h"

#include "../core/DataSerialiser.h"
#include "../peep/Peep.h"
#include "../ride/Vehicle.h"
#include "Sprite.h"

static void EntityBaseSerialise(SpriteBase& base, DataSerialiser& stream)
{
    stream << base.Type;
    stream << base.sprite_index;
    stream << base.x;
    stream << base.y;
    stream << base.z;
    stream << base.sprite_direction;
}

void Litter::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << SubType;
    stream << creationTick;
}

void Balloon::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
    stream << popped;
    stream << time_to_move;
    stream << colour;
}

void Duck::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
    stream << target_x;
    stream << target_y;
    stream << state;
}

void MoneyEffect::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
    stream << MoveDelay;
    stream << NumMovements;
    stream << Vertical;
    stream << Value;
    stream << OffsetX;
    stream << Wiggle;
}

void VehicleCrashParticle::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
    stream << time_to_live;
    stream << colour;
    stream << crashed_sprite_base;
    stream << velocity_x;
    stream << velocity_y;
    stream << velocity_z;
    stream << acceleration_x;
    stream << acceleration_y;
    stream << acceleration_z;
}

void ExplosionFlare::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
}

void ExplosionCloud::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
}

void CrashSplashParticle::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
}

void SteamParticle::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
    stream << time_to_move;
}

static void PeepBaseSerialise(Peep& base, DataSerialiser& stream)
{
    EntityBaseSerialise(base, stream);
    if (stream.IsLoading())
    {
        base.Name = nullptr;
    }
    CoordsXYZ NextLoc;
    uint8_t NextFlags;
    PeepState State;
    union
    {
        uint8_t SubState;
        PeepSittingSubState SittingSubState;
        PeepRideSubState RideSubState;
        PeepUsingBinSubState UsingBinSubState;
    };
    PeepSpriteType SpriteType;
    uint8_t TshirtColour;
    uint8_t TrousersColour;
    uint16_t DestinationX; // Location that the peep is trying to get to
    uint16_t DestinationY;
    uint8_t DestinationTolerance; // How close to destination before next action/state 0 = exact
    uint8_t Var37;
    uint8_t Energy;
    uint8_t EnergyTarget;
    uint8_t Mass;
    uint8_t WindowInvalidateFlags;
    union
    {
        ride_id_t CurrentRide;
        ParkEntranceIndex ChosenParkEntrance;
    };
    StationIndex CurrentRideStation;
    uint8_t CurrentTrain;
    union
    {
        struct
        {
            uint8_t CurrentCar;
            uint8_t CurrentSeat;
        };
        uint16_t TimeToSitdown;
        struct
        {
            uint8_t TimeToStand;
            uint8_t StandingFlags;
        };
    };
    // Normally 0, 1 for carrying sliding board on spiral slide ride, 2 for carrying lawn mower
    uint8_t SpecialSprite;
    PeepActionSpriteType ActionSpriteType;
    // Seems to be used like a local variable, as it's always set before calling SwitchNextActionSpriteType, which
    // reads this again
    PeepActionSpriteType NextActionSpriteType;
    uint8_t ActionSpriteImageOffset;
    PeepActionType Action;
    uint8_t ActionFrame;
    uint8_t StepProgress;
    union
    {
        uint8_t MazeLastEdge;
        Direction PeepDirection; // Direction ?
    };
    ride_id_t InteractionRideIndex;
    uint32_t Id;
    uint8_t PathCheckOptimisation; // see peep.checkForPath
    rct12_xyzd8 PathfindGoal;
    rct12_xyzd8 PathfindHistory[4];
    uint8_t WalkingFrameNum;
    uint32_t PeepFlags;
}

void Guest::Serialise(DataSerialiser& stream)
{
    PeepBaseSerialise(*this, stream);
    uint8_t GuestNumRides;
    uint16_t GuestNextInQueue;
    int32_t ParkEntryTime;
    ride_id_t GuestHeadingToRideId;
    uint8_t GuestIsLostCountdown;
    uint8_t GuestTimeOnRide;
    money16 PaidToEnter;
    money16 PaidOnRides;
    money16 PaidOnFood;
    money16 PaidOnDrink;
    money16 PaidOnSouvenirs;
    bool OutsideOfPark;
    uint8_t Happiness;
    uint8_t HappinessTarget;
    uint8_t Nausea;
    uint8_t NauseaTarget;
    uint8_t Hunger;
    uint8_t Thirst;
    uint8_t Toilet;
    uint8_t TimeToConsume;
    IntensityRange Intensity{ 0 };
    PeepNauseaTolerance NauseaTolerance;
    uint8_t RideTypesBeenOn[16];
    uint16_t TimeInQueue;
    // 255 bit bitmap of every ride the peep has been on see
    // window_peep_rides_update for how to use.
    uint8_t RidesBeenOn[32];
    money32 CashInPocket;
    money32 CashSpent;
    ride_id_t Photo1RideRef;
    ride_id_t Photo2RideRef;
    ride_id_t Photo3RideRef;
    ride_id_t Photo4RideRef;

    int8_t RejoinQueueTimeout; // whilst waiting for a free vehicle (or pair) in the entrance
    ride_id_t PreviousRide;
    uint16_t PreviousRideTimeOut;
    rct_peep_thought Thoughts[PEEP_MAX_THOUGHTS];
    // 0x3F Litter Count split into lots of 3 with time, 0xC0 Time since last recalc
    uint8_t LitterCount;
    // 0x3F Sick Count split into lots of 3 with time, 0xC0 Time since last recalc
    uint8_t DisgustingCount;
    uint8_t AmountOfFood;
    uint8_t AmountOfDrinks;
    uint8_t AmountOfSouvenirs;
    uint8_t VandalismSeen; // 0xC0 vandalism thought timeout, 0x3F vandalism tiles seen
    uint8_t VoucherType;
    union
    {
        ride_id_t VoucherRideId;
        ShopItemIndex VoucherShopItem;
    };
    uint8_t SurroundingsThoughtTimeout;
    uint8_t Angriness;
    uint8_t TimeLost; // the time the peep has been lost when it reaches 254 generates the lost thought
    uint8_t DaysInQueue;
    uint8_t BalloonColour;
    uint8_t UmbrellaColour;
    uint8_t HatColour;
    ride_id_t FavouriteRide;
    uint8_t FavouriteRideRating;
    uint64_t ItemFlags;
}

void Staff::Serialise(DataSerialiser& stream)
{
    PeepBaseSerialise(*this, stream);
    StaffType AssignedStaffType;
    uint16_t MechanicTimeSinceCall; // time getting to ride to fix
    int32_t HireDate;
    uint8_t StaffId;
    uint8_t StaffOrders;
    uint8_t StaffMowingTimeout;
    union
    {
        uint16_t StaffLawnsMown;
        uint16_t StaffRidesFixed;
    };
    union
    {
        uint16_t StaffGardensWatered;
        uint16_t StaffRidesInspected;
    };
    union
    {
        uint16_t StaffLitterSwept;
        uint16_t StaffVandalsStopped;
    };
    uint16_t StaffBinsEmptied;
}

void Vehicle::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    Type SubType;
    uint8_t vehicle_sprite_type;
    uint8_t bank_rotation;
    int32_t remaining_distance;
    int32_t velocity;
    int32_t acceleration;
    ride_id_t ride;
    uint8_t vehicle_type;
    rct_vehicle_colour colours;
    union
    {
        uint16_t track_progress;
        struct
        {
            int8_t var_34;
            uint8_t var_35;
        };
    };
    uint16_t TrackTypeAndDirection;
    CoordsXYZ TrackLocation;
    uint16_t next_vehicle_on_train;

    // The previous vehicle on the same train or the last vehicle on the previous or only train.
    uint16_t prev_vehicle_on_ride;

    // The next vehicle on the same train or the first vehicle on the next or only train
    uint16_t next_vehicle_on_ride;

    uint16_t var_44;
    uint16_t mass;
    uint16_t update_flags;
    uint8_t SwingSprite;
    StationIndex current_station;
    union
    {
        int16_t SwingPosition;
        int16_t current_time;
        struct
        {
            int8_t ferris_wheel_var_0;
            int8_t ferris_wheel_var_1;
        };
    };
    union
    {
        int16_t SwingSpeed;
        int16_t crash_z;
    };
    Status status;
    uint8_t sub_state;
    uint16_t peep[32];
    uint8_t peep_tshirt_colours[32];
    uint8_t num_seats;
    uint8_t num_peeps;
    uint8_t next_free_seat;
    uint8_t restraints_position; // 0 == Close, 255 == Open
    union
    {
        int16_t spin_speed;
        int16_t crash_x;
    };
    uint16_t sound2_flags;
    uint8_t spin_sprite; // lowest 3 bits not used for sprite selection (divide by 8 to use)
    OpenRCT2::Audio::SoundId sound1_id;
    uint8_t sound1_volume;
    OpenRCT2::Audio::SoundId sound2_id;
    uint8_t sound2_volume;
    int8_t sound_vector_factor;
    union
    {
        uint16_t var_C0;
        int16_t crash_y;
        uint16_t time_waiting;
        uint16_t cable_lift_target;
    };
    uint8_t speed;
    uint8_t powered_acceleration;
    union
    {
        uint8_t dodgems_collision_direction;
        uint8_t var_C4;
    };
    uint8_t animation_frame;
    uint8_t pad_C6[0x2];
    uint16_t var_C8;
    uint16_t var_CA;
    OpenRCT2::Audio::SoundId scream_sound_id;
    VehicleTrackSubposition TrackSubposition;
    union
    {
        uint8_t var_CE;
        uint8_t num_laps;
    };
    union
    {
        uint8_t var_CF;
        uint8_t brake_speed;
    };
    uint16_t lost_time_out;
    int8_t vertical_drop_countdown;
    uint8_t var_D3;
    uint8_t mini_golf_current_animation;
    uint8_t mini_golf_flags;
    ObjectEntryIndex ride_subtype;
    uint8_t colours_extended;
    uint8_t seat_rotation;
    uint8_t target_seat_rotation;
    CoordsXY BoatLocation;
    bool IsCrashedVehicle;
}
