/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _VEHICLE_H_
#define _VEHICLE_H_

#include "../audio/audio.h"
#include "../common.h"
#include "../ride/RideTypes.h"
#include "../world/Location.hpp"
#include "../world/SpriteBase.h"
#include "Station.h"
#include "VehicleColour.h"
#include "VehicleEntry.h"
#include "VehicleSubpositionData.h"

#include <array>
#include <cstddef>
#include <vector>

using track_type_t = uint16_t;

struct Ride;
struct rct_ride_entry;

struct GForces
{
    int32_t VerticalG{};
    int32_t LateralG{};
};

// Size: 0x09
struct rct_vehicle_info
{
    int16_t x;                   // 0x00
    int16_t y;                   // 0x02
    int16_t z;                   // 0x04
    uint8_t direction;           // 0x06
    uint8_t vehicle_sprite_type; // 0x07
    uint8_t bank_rotation;       // 0x08
};

struct SoundIdVolume;

constexpr const uint16_t VehicleTrackDirectionMask = 0b0000000000000011;
constexpr const uint16_t VehicleTrackTypeMask = 0b1111111111111100;

struct Vehicle : SpriteBase
{
    enum class Type : uint8_t
    {
        Head,
        Tail,
    };

    enum class Status
    {
        MovingToEndOfStation,
        WaitingForPassengers,
        WaitingToDepart,
        Departing,
        Travelling,
        Arriving,
        UnloadingPassengers,
        TravellingBoat,
        Crashing,
        Crashed,
        TravellingDodgems,
        Swinging,
        Rotating,
        FerrisWheelRotating,
        SimulatorOperating,
        ShowingFilm,
        SpaceRingsOperating,
        TopSpinOperating,
        HauntedHouseOperating,
        DoingCircusShow,
        CrookedHouseOperating,
        WaitingForCableLift,
        TravellingCableLift,
        Stopping,
        WaitingForPassengers17,
        WaitingToStart,
        Starting,
        Operating1A,
        Stopping1B,
        UnloadingPassengers1C,
        StoppedByBlockBrakes
    };

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

    constexpr bool IsHead() const
    {
        return SubType == Vehicle::Type::Head;
    }
    void Update();
    Vehicle* GetHead();
    const Vehicle* GetHead() const;
    Vehicle* GetCar(size_t carIndex) const;
    void SetState(Vehicle::Status vehicleStatus, uint8_t subState = 0);
    bool IsGhost() const;
    void UpdateSoundParams(std::vector<OpenRCT2::Audio::VehicleSoundParams>& vehicleSoundParamsList) const;
    bool DodgemsCarWouldCollideAt(const CoordsXY& coords, uint16_t* spriteId) const;
    int32_t UpdateTrackMotion(int32_t* outStation);
    int32_t CableLiftUpdateTrackMotion();
    GForces GetGForces() const;
    void SetMapToolbar() const;
    int32_t IsUsedInPairs() const;
    rct_ride_entry* GetRideEntry() const;
    rct_ride_entry_vehicle* Entry() const;
    Ride* GetRide() const;
    Vehicle* TrainHead() const;
    Vehicle* TrainTail() const;
    void EnableCollisionsForTrain();
    /**
     * Instantly moves the specific car forward or backwards along the track.
     */
    void MoveRelativeDistance(int32_t distance);
    track_type_t GetTrackType() const
    {
        return TrackTypeAndDirection >> 2;
    }
    uint8_t GetTrackDirection() const
    {
        return TrackTypeAndDirection & VehicleTrackDirectionMask;
    }
    void SetTrackType(track_type_t trackType)
    {
        // set the upper 14 bits to 0, then set track type
        TrackTypeAndDirection &= ~VehicleTrackTypeMask;
        TrackTypeAndDirection |= trackType << 2;
    }
    void SetTrackDirection(uint8_t trackDirection)
    {
        // set the lower 2 bits only
        TrackTypeAndDirection &= ~VehicleTrackDirectionMask;
        TrackTypeAndDirection |= trackDirection & VehicleTrackDirectionMask;
    }
    bool HasUpdateFlag(uint32_t flag) const
    {
        return (update_flags & flag) != 0;
    }
    void ClearUpdateFlag(uint32_t flag)
    {
        update_flags &= ~flag;
    }
    void SetUpdateFlag(uint32_t flag)
    {
        update_flags |= flag;
    }
    void ApplyMass(int16_t appliedMass);

private:
    bool SoundCanPlay() const;
    uint16_t GetSoundPriority() const;
    const rct_vehicle_info* GetMoveInfo() const;
    uint16_t GetTrackProgress() const;
    OpenRCT2::Audio::VehicleSoundParams CreateSoundParam(uint16_t priority) const;
    void CableLiftUpdate();
    bool CableLiftUpdateTrackMotionForwards();
    bool CableLiftUpdateTrackMotionBackwards();
    void CableLiftUpdateMovingToEndOfStation();
    void CableLiftUpdateWaitingToDepart();
    void CableLiftUpdateDeparting();
    void CableLiftUpdateTravelling();
    void CableLiftUpdateArriving();
    void Sub6DBF3E();
    void UpdateMeasurements();
    void UpdateMovingToEndOfStation();
    void UpdateWaitingForPassengers();
    void UpdateWaitingToDepart();
    void UpdateCrash();
    void UpdateDodgemsMode();
    void UpdateSwinging();
    void UpdateSimulatorOperating();
    void UpdateTopSpinOperating();
    void UpdateFerrisWheelRotating();
    void UpdateSpaceRingsOperating();
    void UpdateHauntedHouseOperating();
    void UpdateCrookedHouseOperating();
    void UpdateRotating();
    void UpdateDeparting();
    void FinishDeparting();
    void UpdateTravelling();
    void UpdateTravellingCableLift();
    void UpdateTravellingBoat();
    void UpdateMotionBoatHire();
    void TryReconnectBoatToTrack(const CoordsXY& currentBoatLocation, const CoordsXY& trackCoords);
    void UpdateDepartingBoatHire();
    void UpdateTravellingBoatHireSetup();
    void UpdateBoatLocation();
    void UpdateArriving();
    void UpdateUnloadingPassengers();
    void UpdateWaitingForCableLift();
    void UpdateShowingFilm();
    void UpdateDoingCircusShow();
    void UpdateCrossings() const;
    void UpdateSound();
    void GetLiftHillSound(Ride* curRide, SoundIdVolume& curSound);
    OpenRCT2::Audio::SoundId UpdateScreamSound();
    OpenRCT2::Audio::SoundId ProduceScreamSound(const int32_t totalNumPeeps);
    void UpdateCrashSetup();
    void UpdateCollisionSetup();
    int32_t UpdateMotionDodgems();
    void UpdateAdditionalAnimation();
    void CheckIfMissing();
    bool CurrentTowerElementIsTop();
    bool UpdateTrackMotionForwards(rct_ride_entry_vehicle* vehicleEntry, Ride* curRide, rct_ride_entry* rideEntry);
    bool UpdateTrackMotionBackwards(rct_ride_entry_vehicle* vehicleEntry, Ride* curRide, rct_ride_entry* rideEntry);
    int32_t UpdateTrackMotionPoweredRideAcceleration(
        rct_ride_entry_vehicle* vehicleEntry, uint32_t totalMass, const int32_t curAcceleration);
    int32_t NumPeepsUntilTrainTail() const;
    void InvalidateWindow();
    void TestReset();
    void UpdateTestFinish();
    void PeepEasterEggHereWeAre() const;
    bool CanDepartSynchronised() const;
    void ReverseReverserCar();
    void UpdateReverserCarBogies();
    void UpdateHandleWaterSplash() const;
    void Claxon() const;
    void UpdateTrackMotionUpStopCheck() const;
    void ApplyNonStopBlockBrake();
    void ApplyStopBlockBrake();
    void CheckAndApplyBlockSectionStopSite();
    void UpdateVelocity();
    void UpdateSpinningCar();
    void UpdateSwingingCar();
    int32_t GetSwingAmount() const;
    bool OpenRestraints();
    bool CloseRestraints();
    void CrashOnWater();
    void CrashOnLand();
    void SimulateCrash() const;
    void KillAllPassengersInTrain();
    void KillPassengers(Ride* curRide);
    void TrainReadyToDepart(uint8_t num_peeps_on_train, uint8_t num_used_seats);
    int32_t UpdateTrackMotionMiniGolf(int32_t* outStation);
    void UpdateTrackMotionMiniGolfVehicle(Ride* curRide, rct_ride_entry* rideEntry, rct_ride_entry_vehicle* vehicleEntry);
    bool UpdateTrackMotionForwardsGetNewTrack(uint16_t trackType, Ride* curRide, rct_ride_entry* rideEntry);
    bool UpdateTrackMotionBackwardsGetNewTrack(uint16_t trackType, Ride* curRide, uint16_t* progress);
    bool UpdateMotionCollisionDetection(const CoordsXYZ& loc, uint16_t* otherVehicleIndex);
    void UpdateGoKartAttemptSwitchLanes();
    void UpdateSceneryDoor() const;
    void UpdateSceneryDoorBackwards() const;
    void UpdateLandscapeDoor() const;
    void UpdateLandscapeDoorBackwards() const;
};

struct train_ref
{
    Vehicle* head;
    Vehicle* tail;
};

enum : uint32_t
{
    VEHICLE_ENTRY_FLAG_POWERED_RIDE_UNRESTRICTED_GRAVITY = 1
        << 0, // Set on powered vehicles that do not slow down when going down a hill
    VEHICLE_ENTRY_FLAG_NO_UPSTOP_WHEELS = 1 << 1,
    VEHICLE_ENTRY_FLAG_NO_UPSTOP_BOBSLEIGH = 1 << 2,
    VEHICLE_ENTRY_FLAG_MINI_GOLF = 1 << 3,
    VEHICLE_ENTRY_FLAG_4 = 1 << 4,
    VEHICLE_ENTRY_FLAG_5 = 1 << 5,
    VEHICLE_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET = 1 << 6, // Set on vehicles that support running inverted for extended periods
                                                         // of time, i.e. the Flying, Lay-down and Multi-dimension RCs.
    VEHICLE_ENTRY_FLAG_DODGEM_INUSE_LIGHTS = 1
        << 7, // When set the vehicle has an additional frame for when in use. Used only by dodgems.
    VEHICLE_ENTRY_FLAG_ALLOW_DOORS_DEPRECATED = 1 << 8, // Not used any more - every vehicle will now work with doors
    VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_2 = 1 << 9,
    VEHICLE_ENTRY_FLAG_10 = 1 << 10,
    VEHICLE_ENTRY_FLAG_11 = 1 << 11,
    VEHICLE_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES = 1
        << 12, // Setting this will cause the game to set vehicleEntry->num_vertical_frames to
               // vehicleEntry->num_vertical_frames_override, rather than determining it itself.
    VEHICLE_ENTRY_FLAG_13 = 1 << 13,
    VEHICLE_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES = 1
        << 14, // 16x additional frames for vehicle. A spinning item with additional frames must always face forward to
               // load/unload. Spinning without can load/unload at 4 rotations.
    VEHICLE_ENTRY_FLAG_LIFT = 1 << 15,
    VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_1 = 1 << 16,
    VEHICLE_ENTRY_FLAG_SWINGING = 1 << 17,
    VEHICLE_ENTRY_FLAG_SPINNING = 1 << 18,
    VEHICLE_ENTRY_FLAG_POWERED = 1 << 19,
    VEHICLE_ENTRY_FLAG_RIDERS_SCREAM = 1 << 20,
    VEHICLE_ENTRY_FLAG_21 = 1 << 21, // Swinging coaster??
    VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION = 1 << 22,
    VEHICLE_ENTRY_FLAG_VEHICLE_ANIMATION = 1 << 23, // Set on animated vehicles like the Multi-dimension coaster trains,
                                                    // Miniature Railway locomotives and Helicycles.
    VEHICLE_ENTRY_FLAG_RIDER_ANIMATION = 1 << 24,   // Set when the animation updates rider sprite positions
    VEHICLE_ENTRY_FLAG_25 = 1 << 25,
    VEHICLE_ENTRY_FLAG_LOADING_WAYPOINTS = 1
        << 26, // Peep loading positions have x and y coordinates. Normal rides just have offsets
    VEHICLE_ENTRY_FLAG_SLIDE_SWING = 1
        << 27, // Set on dingy slides. They have there own swing value calculations and have a different amount of images.
    VEHICLE_ENTRY_FLAG_CHAIRLIFT = 1 << 28,
    VEHICLE_ENTRY_FLAG_WATER_RIDE = 1 << 29, // Set on rides where water would provide continuous propulsion
    VEHICLE_ENTRY_FLAG_GO_KART = 1 << 30,
    VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT = 1u << 31,
};

enum
{
    VEHICLE_ENTRY_ANIMATION_NONE,
    VEHICLE_ENTRY_ANIMATION_MINITURE_RAILWAY_LOCOMOTIVE,
    VEHICLE_ENTRY_ANIMATION_SWAN,
    VEHICLE_ENTRY_ANIMATION_CANOES,
    VEHICLE_ENTRY_ANIMATION_ROW_BOATS,
    VEHICLE_ENTRY_ANIMATION_WATER_TRICYCLES,
    VEHICLE_ENTRY_ANIMATION_OBSERVATION_TOWER,
    VEHICLE_ENTRY_ANIMATION_HELICARS,
    VEHICLE_ENTRY_ANIMATION_MONORAIL_CYCLES,
    VEHICLE_ENTRY_ANIMATION_MULTI_DIM_COASTER
};

enum : uint32_t
{
    VEHICLE_UPDATE_FLAG_ON_LIFT_HILL = (1 << 0),
    VEHICLE_UPDATE_FLAG_COLLISION_DISABLED = (1 << 1),
    VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT = (1 << 2),
    VEHICLE_UPDATE_FLAG_REVERSING_SHUTTLE = (1 << 3), // Shuttle is in reverse
    VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART = (1 << 4),
    VEHICLE_UPDATE_FLAG_TESTING = (1 << 5),
    VEHICLE_UPDATE_FLAG_6 = (1 << 6),
    VEHICLE_UPDATE_FLAG_ZERO_VELOCITY = (1 << 7), // Used on rides when safety cutout stops them on a lift
    VEHICLE_UPDATE_FLAG_BROKEN_CAR = (1 << 8),
    VEHICLE_UPDATE_FLAG_BROKEN_TRAIN = (1 << 9),
    VEHICLE_UPDATE_FLAG_ON_BRAKE_FOR_DROP = (1 << 10),
    VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES = (1 << 11), // Used on rides where trains can run for extended periods of time,
                                                          // i.e. the Flying, Lay-down and Multi-dimension RCs.
    VEHICLE_UPDATE_FLAG_12 = (1 << 12),
    VEHICLE_UPDATE_FLAG_ROTATION_OFF_WILD_MOUSE = (1 << 13), // After passing a rotation toggle track piece this will enable
    VEHICLE_UPDATE_FLAG_SINGLE_CAR_POSITION = (1 << 14), // OpenRCT2 Flag: Used to override UpdateMotion to move the position of
                                                         // an individual car on a train
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
    VEHICLE_SPRITE_FLAG_15 = (1 << 15),
};

enum
{
    VEHICLE_VISUAL_DEFAULT,
    VEHICLE_VISUAL_FLAT_RIDE_OR_CAR_RIDE,
    VEHICLE_VISUAL_LAUNCHED_FREEFALL,
    VEHICLE_VISUAL_OBSERVATION_TOWER,
    VEHICLE_VISUAL_RIVER_RAPIDS,
    VEHICLE_VISUAL_MINI_GOLF_PLAYER,
    VEHICLE_VISUAL_MINI_GOLF_BALL,
    VEHICLE_VISUAL_REVERSER,
    VEHICLE_VISUAL_SPLASH_BOATS_OR_WATER_COASTER,
    VEHICLE_VISUAL_ROTO_DROP,
    VEHICLE_VISUAL_SPLASH1_EFFECT,
    VEHICLE_VISUAL_SPLASH2_EFFECT,
    VEHICLE_VISUAL_SPLASH3_EFFECT,
    VEHICLE_VISUAL_SPLASH4_EFFECT,
    VEHICLE_VISUAL_SPLASH5_EFFECT,
    VEHICLE_VISUAL_VIRGINIA_REEL,
    VEHICLE_VISUAL_SUBMARINE
};

enum : uint32_t
{
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION = 1 << 0,
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_1 = 1 << 1,
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_2 = 1 << 2,
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_3 = 1 << 3,
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL = 1 << 4,
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_5 = 1 << 5,
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_DERAILED = 1 << 6,
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION = 1 << 7,
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_8 = 1 << 8,
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_9 = 1 << 9,
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_BLOCK_BRAKE = 1 << 10,
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_11 = 1 << 11,
    VEHICLE_UPDATE_MOTION_TRACK_FLAG_12 = 1 << 12,
};

enum
{
    VEHICLE_SOUND2_FLAGS_LIFT_HILL = 1 << 1 // When on a lift hill generate sound
};

enum
{
    FRICTION_SOUND_WOOD_SMALL = 1,
    FRICTION_SOUND_STEEL = 2,
    FRICTION_SOUND_PETROL = 21,
    FRICTION_SOUND_TRAIN = 31,
    FRICTION_SOUND_WATERSLIDE = 32,
    FRICTION_SOUND_WOOD_LARGE = 54,
    FRICTION_SOUND_STEEL_SMOOTH = 57,
    FRICTION_SOUND_NONE = 255
};

enum
{
    SOUND_RANGE_SCREAMS_0 = 0,
    SOUND_RANGE_SCREAMS_1 = 1,
    SOUND_RANGE_SCREAMS_2 = 2,
    SOUND_RANGE_WHISTLE = 3,
    SOUND_RANGE_BELL = 4,
    SOUND_RANGE_NONE = 255
};

#define VEHICLE_SEAT_PAIR_FLAG 0x80
#define VEHICLE_SEAT_NUM_MASK 0x7F

Vehicle* try_get_vehicle(uint16_t spriteIndex);
void vehicle_update_all();
void vehicle_sounds_update();

extern Vehicle* gCurrentVehicle;
extern StationIndex _vehicleStationIndex;
extern uint32_t _vehicleMotionTrackFlags;
extern int32_t _vehicleVelocityF64E08;
extern int32_t _vehicleVelocityF64E0C;
extern int32_t _vehicleUnkF64E10;
extern uint8_t _vehicleF64E2C;
extern Vehicle* _vehicleFrontVehicle;
extern CoordsXYZ unk_F64E20;

#endif
