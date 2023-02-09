/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../audio/audio.h"
#include "../common.h"
#include "../entity/EntityBase.h"
#include "../ride/RideTypes.h"
#include "../world/Location.hpp"
#include "CarEntry.h"
#include "Station.h"
#include "VehicleColour.h"
#include "VehicleSubpositionData.h"

#include <array>
#include <cstddef>
#include <vector>

using track_type_t = uint16_t;

struct Ride;
struct RideObjectEntry;
struct CarEntry;
class DataSerialiser;
struct PaintSession;

struct GForces
{
    int32_t VerticalG{};
    int32_t LateralG{};
};

// Size: 0x09
struct VehicleInfo
{
    int16_t x;             // 0x00
    int16_t y;             // 0x02
    int16_t z;             // 0x04
    uint8_t direction;     // 0x06
    uint8_t Pitch;         // 0x07
    uint8_t bank_rotation; // 0x08
};

struct SoundIdVolume;

constexpr const uint16_t VehicleTrackDirectionMask = 0b0000000000000011;
constexpr const uint16_t VehicleTrackTypeMask = 0b1111111111111100;

enum class MiniGolfAnimation : uint8_t;

struct Vehicle : EntityBase
{
    static constexpr auto cEntityType = EntityType::Vehicle;

    enum class Type : uint8_t
    {
        Head,
        Tail,
    };

    enum class Status : uint8_t
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
    uint8_t Pitch;
    uint8_t bank_rotation;
    int32_t remaining_distance;
    int32_t velocity;
    int32_t acceleration;
    RideId ride;
    uint8_t vehicle_type;
    VehicleColour colours;
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
    EntityId next_vehicle_on_train;

    // The previous vehicle on the same train or the last vehicle on the previous or only train.
    EntityId prev_vehicle_on_ride;

    // The next vehicle on the same train or the first vehicle on the next or only train
    EntityId next_vehicle_on_ride;

    uint16_t var_44;
    uint16_t mass;
    uint32_t Flags;
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
        int16_t rider_speed_preference;
    };
    union
    {
        int16_t SwingSpeed;
        int16_t crash_z;
    };
    Status status;
    uint8_t sub_state;
    EntityId peep[32];
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
        EntityId cable_lift_target;
    };
    uint8_t speed;
    uint8_t powered_acceleration;
    union
    {
        uint8_t DodgemsCollisionDirection;
        uint8_t CollisionDetectionTimer;
    };
    uint8_t animation_frame;
    uint8_t PadC6[0x2];
    uint32_t animationState;
    OpenRCT2::Audio::SoundId scream_sound_id;
    VehicleTrackSubposition TrackSubposition;
    union
    {
        uint8_t NumLaps;
        uint8_t NumSwings;
        uint8_t NumLaunches;
        uint8_t NumRotations;
        uint8_t TimeActive;
    };
    uint8_t brake_speed;
    uint16_t lost_time_out;
    int8_t vertical_drop_countdown;
    uint8_t var_D3;
    MiniGolfAnimation mini_golf_current_animation;
    uint8_t mini_golf_flags;
    ObjectEntryIndex ride_subtype;
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
    std::optional<EntityId> DodgemsCarWouldCollideAt(const CoordsXY& coords) const;
    int32_t UpdateTrackMotion(int32_t* outStation);
    int32_t CableLiftUpdateTrackMotion();
    GForces GetGForces() const;
    void SetMapToolbar() const;
    int32_t IsUsedInPairs() const;
    const RideObjectEntry* GetRideEntry() const;
    const CarEntry* Entry() const;
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
        return (Flags & flag) != 0;
    }
    void ClearUpdateFlag(uint32_t flag)
    {
        Flags &= ~flag;
    }
    void SetUpdateFlag(uint32_t flag)
    {
        Flags |= flag;
    }
    void ApplyMass(int16_t appliedMass);
    void Serialise(DataSerialiser& stream);
    void Paint(PaintSession& session, int32_t imageDirection) const;
    bool IsCableLift() const;

    friend void UpdateRotatingDefault(Vehicle& vehicle);
    friend void UpdateRotatingEnterprise(Vehicle& vehicle);

private:
    bool SoundCanPlay() const;
    uint16_t GetSoundPriority() const;
    const VehicleInfo* GetMoveInfo() const;
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
    void UpdateArrivingPassThroughStation(const Ride& curRide, const CarEntry& carEntry, bool stationBrakesWork);
    void UpdateArriving();
    void UpdateUnloadingPassengers();
    void UpdateWaitingForCableLift();
    void UpdateShowingFilm();
    void UpdateDoingCircusShow();
    void UpdateCrossings() const;
    void UpdateSound();
    void GetLiftHillSound(const Ride& curRide, SoundIdVolume& curSound);
    OpenRCT2::Audio::SoundId UpdateScreamSound();
    OpenRCT2::Audio::SoundId ProduceScreamSound(const int32_t totalNumPeeps);
    void UpdateCrashSetup();
    void UpdateCollisionSetup();
    int32_t UpdateMotionDodgems();
    void UpdateAnimationAnimalFlying();
    void UpdateAdditionalAnimation();
    void CheckIfMissing();
    bool CurrentTowerElementIsTop();
    bool UpdateTrackMotionForwards(const CarEntry* carEntry, const Ride& curRide, const RideObjectEntry& rideEntry);
    bool UpdateTrackMotionBackwards(const CarEntry* carEntry, const Ride& curRide, const RideObjectEntry& rideEntry);
    int32_t UpdateTrackMotionPoweredRideAcceleration(
        const CarEntry* carEntry, uint32_t totalMass, const int32_t curAcceleration);
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
    void KillPassengers(const Ride& curRide);
    void TrainReadyToDepart(uint8_t num_peeps_on_train, uint8_t num_used_seats);
    int32_t UpdateTrackMotionMiniGolfCalculateAcceleration(const CarEntry& carEntry);
    int32_t UpdateTrackMotionMiniGolf(int32_t* outStation);
    void UpdateTrackMotionMiniGolfVehicle(const Ride& curRide, const RideObjectEntry& rideEntry, const CarEntry* carEntry);
    bool UpdateTrackMotionForwardsGetNewTrack(uint16_t trackType, const Ride& curRide, const RideObjectEntry& rideEntry);
    bool UpdateTrackMotionBackwardsGetNewTrack(uint16_t trackType, const Ride& curRide, uint16_t* progress);
    bool UpdateMotionCollisionDetection(const CoordsXYZ& loc, EntityId* otherVehicleIndex);
    void UpdateGoKartAttemptSwitchLanes();
    void UpdateSceneryDoor() const;
    void UpdateSceneryDoorBackwards() const;
    void UpdateLandscapeDoor() const;
    void UpdateLandscapeDoorBackwards() const;
    int32_t CalculateRiderBraking() const;

    void Loc6DCE02(const Ride& curRide);
};
static_assert(sizeof(Vehicle) <= 512);

void UpdateRotatingDefault(Vehicle& vehicle);
void UpdateRotatingEnterprise(Vehicle& vehicle);

struct TrainReference
{
    Vehicle* head;
    Vehicle* tail;
};

namespace MiniGolfFlag
{
    constexpr uint8_t Flag0 = (1 << 0);
    constexpr uint8_t Flag1 = (1 << 1);
    constexpr uint8_t Flag2 = (1 << 2);
    constexpr uint8_t Flag3 = (1 << 3);
    constexpr uint8_t Flag4 = (1 << 4);
    constexpr uint8_t Flag5 = (1 << 5); // transitioning between hole
} // namespace MiniGolfFlag

enum class MiniGolfState : int16_t
{
    Unk0,
    Unk1, // Unused
    Unk2,
    Unk3,
    Unk4,
    Unk5,
    Unk6,
};

enum class MiniGolfAnimation : uint8_t
{
    Walk,
    PlaceBallDown,
    SwingLeft,
    PickupBall,
    Jump,
    PlaceBallUp,
    PuttLeft,
    Swing,
    Putt,
};

enum
{
    CAR_ENTRY_ANIMATION_NONE,
    CAR_ENTRY_ANIMATION_MINITURE_RAILWAY_LOCOMOTIVE,
    CAR_ENTRY_ANIMATION_SWAN,
    CAR_ENTRY_ANIMATION_CANOES,
    CAR_ENTRY_ANIMATION_ROW_BOATS,
    CAR_ENTRY_ANIMATION_WATER_TRICYCLES,
    CAR_ENTRY_ANIMATION_OBSERVATION_TOWER,
    CAR_ENTRY_ANIMATION_HELICARS,
    CAR_ENTRY_ANIMATION_MONORAIL_CYCLES,
    CAR_ENTRY_ANIMATION_MULTI_DIM_COASTER,
    CAR_ENTRY_ANIMATION_ANIMAL_FLYING // OpenRCT2-specific feature
};

enum : uint32_t
{
    VEHICLE_FLAG_ON_LIFT_HILL = (1 << 0),
    VEHICLE_FLAG_COLLISION_DISABLED = (1 << 1),
    VEHICLE_FLAG_WAIT_ON_ADJACENT = (1 << 2),
    VEHICLE_FLAG_REVERSING_SHUTTLE = (1 << 3), // Shuttle is in reverse
    VEHICLE_FLAG_TRAIN_READY_DEPART = (1 << 4),
    VEHICLE_FLAG_TESTING = (1 << 5),
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
    VEHICLE_UPDATE_FLAG_CRASHED = (1 << 15),
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
    FRICTION_SOUND_HYBRID = 65,
    FRICTION_SOUND_NONE = 255
};

enum
{
    SOUND_RANGE_SCREAMS_0 = 0,
    SOUND_RANGE_SCREAMS_1_WOODEN_COASTERS = 1,
    SOUND_RANGE_SCREAMS_2 = 2,
    SOUND_RANGE_WHISTLE = 3,
    SOUND_RANGE_BELL = 4,
    SOUND_RANGE_NONE = 255
};

#define VEHICLE_SEAT_PAIR_FLAG 0x80
#define VEHICLE_SEAT_NUM_MASK 0x7F

Vehicle* TryGetVehicle(EntityId spriteIndex);
void VehicleUpdateAll();
void VehicleSoundsUpdate();
uint16_t VehicleGetMoveInfoSize(VehicleTrackSubposition trackSubposition, track_type_t type, uint8_t direction);

void RideUpdateMeasurementsSpecialElements_Default(Ride& ride, const track_type_t trackType);
void RideUpdateMeasurementsSpecialElements_MiniGolf(Ride& ride, const track_type_t trackType);
void RideUpdateMeasurementsSpecialElements_WaterCoaster(Ride& ride, const track_type_t trackType);

extern Vehicle* gCurrentVehicle;
extern StationIndex _vehicleStationIndex;
extern uint32_t _vehicleMotionTrackFlags;
extern int32_t _vehicleVelocityF64E08;
extern int32_t _vehicleVelocityF64E0C;
extern int32_t _vehicleUnkF64E10;
extern uint8_t _vehicleF64E2C;
extern Vehicle* _vehicleFrontVehicle;
extern CoordsXYZ _vehicleCurPosition;
