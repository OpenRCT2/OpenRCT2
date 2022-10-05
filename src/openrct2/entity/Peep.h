/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../common.h"
#include "../entity/EntityBase.h"
#include "../ride/RideTypes.h"
#include "../ride/Station.h"
#include "../util/Util.h"
#include "../world/Location.hpp"

#include <algorithm>
#include <array>
#include <optional>

#define PEEP_MIN_ENERGY 32
#define PEEP_MAX_ENERGY 128
#define PEEP_MAX_ENERGY_TARGET 255 // Oddly, this differs from max energy!

constexpr auto PEEP_CLEARANCE_HEIGHT = 4 * COORDS_Z_STEP;

class Formatter;
struct TileElement;
struct paint_session;

namespace GameActions
{
    class Result;
}

enum class StaffType : uint8_t
{
    Handyman,
    Mechanic,
    Security,
    Entertainer,

    Count
};

enum class PeepState : uint8_t
{
    Falling = 0, // Drowning is part of falling
    One = 1,     // was PEEP_STATE_1
    QueuingFront = 2,
    OnRide = 3,
    LeavingRide = 4,
    Walking = 5,
    Queuing = 6,
    EnteringRide = 7,
    Sitting = 8,
    Picked = 9,
    Patrolling = 10, // Not sure
    Mowing = 11,
    Sweeping = 12,
    EnteringPark = 13,
    LeavingPark = 14,
    Answering = 15,
    Fixing = 16,
    Buying = 17,
    Watching = 18,
    EmptyingBin = 19,
    UsingBin = 20,
    Watering = 21,
    HeadingToInspection = 22,
    Inspecting = 23
};

enum class PeepSittingSubState : uint8_t
{
    TryingToSit, // was = 0
    SatDown      // was unassigned
};

enum class PeepRideSubState : uint8_t
{
    AtEntrance = 0,
    InEntrance = 1,
    FreeVehicleCheck = 2, // Spend money on ride
    LeaveEntrance = 3,    // Calculate what direction and where to go after committing to entering vehicle
    ApproachVehicle = 4,
    EnterVehicle = 5,
    OnRide = 6,
    LeaveVehicle = 7,
    ApproachExit = 8,
    InExit = 9,
    InQueue = 10,
    AtQueueFront = 11,
    ApproachVehicleWaypoints = 12,
    ApproachExitWaypoints = 13,
    ApproachSpiralSlide = 14,
    OnSpiralSlide = 15,
    LeaveSpiralSlide = 16,
    MazePathfinding = 17,
    LeaveExit = 18,
    ApproachShop = 19,
    InteractShop = 20,
    LeaveShop = 21
};

enum class PeepUsingBinSubState : uint8_t
{
    WalkingToBin = 0,
    GoingBack,
};

enum class PeepActionType : uint8_t
{
    CheckTime = 0,
    // If no food then check watch
    EatFood = 1,
    ShakeHead = 2,
    EmptyPockets = 3,
    SittingEatFood = 4,
    SittingCheckWatch = 4,
    SittingLookAroundLeft = 5,
    SittingLookAroundRight = 6,
    Wow = 7,
    ThrowUp = 8,
    Jump = 9,
    StaffSweep = 10,
    Drowning = 11,
    StaffAnswerCall = 12,
    StaffAnswerCall2 = 13,
    StaffCheckboard = 14,
    StaffFix = 15,
    StaffFix2 = 16,
    StaffFixGround = 17,
    StaffFix3 = 18,
    StaffWatering = 19,
    Joy = 20,
    ReadMap = 21,
    Wave = 22,
    StaffEmptyBin = 23,
    Wave2 = 24,
    TakePhoto = 25,
    Clap = 26,
    Disgust = 27,
    DrawPicture = 28,
    BeingWatched = 29,
    WithdrawMoney = 30,

    Idle = 254,
    Walking = 255,
};

enum class PeepActionSpriteType : uint8_t
{
    None = 0,
    CheckTime = 1,
    WatchRide = 2,
    EatFood = 3,
    ShakeHead = 4,
    EmptyPockets = 5,
    HoldMat = 6,
    SittingIdle = 7,
    SittingEatFood = 8,
    SittingLookAroundLeft = 9,
    SittingLookAroundRight = 10,
    Ui = 11,
    StaffMower = 12,
    Wow = 13,
    ThrowUp = 14,
    Jump = 15,
    StaffSweep = 16,
    Drowning = 17,
    StaffAnswerCall = 18,
    StaffAnswerCall2 = 19,
    StaffCheckboard = 20,
    StaffFix = 21,
    StaffFix2 = 22,
    StaffFixGround = 23,
    StaffFix3 = 24,
    StaffWatering = 25,
    Joy = 26,
    ReadMap = 27,
    Wave = 28,
    StaffEmptyBin = 29,
    Wave2 = 30,
    TakePhoto = 31,
    Clap = 32,
    Disgust = 33,
    DrawPicture = 34,
    BeingWatched = 35,
    WithdrawMoney = 36,

    Invalid = 255
};

enum PeepFlags : uint32_t
{
    PEEP_FLAGS_LEAVING_PARK = (1 << 0),
    PEEP_FLAGS_SLOW_WALK = (1 << 1),
    PEEP_FLAGS_2 = (1 << 2),
    PEEP_FLAGS_TRACKING = (1 << 3),
    PEEP_FLAGS_WAVING = (1 << 4),                  // Makes the peep wave
    PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY = (1 << 5), // Set on paying to enter park
    PEEP_FLAGS_PHOTO = (1 << 6),                   // Makes the peep take a picture
    PEEP_FLAGS_PAINTING = (1 << 7),
    PEEP_FLAGS_WOW = (1 << 8),        // Makes a peep WOW2
    PEEP_FLAGS_LITTER = (1 << 9),     // Makes the peep throw litter
    PEEP_FLAGS_LOST = (1 << 10),      // Makes the peep feel lost (animation triggered)
    PEEP_FLAGS_HUNGER = (1 << 11),    // Makes the peep become hungry quicker
    PEEP_FLAGS_TOILET = (1 << 12),    // Makes the peep want to go to the toilet
    PEEP_FLAGS_CROWDED = (1 << 13),   // The peep will start feeling crowded
    PEEP_FLAGS_HAPPINESS = (1 << 14), // The peep will start increasing happiness
    PEEP_FLAGS_NAUSEA = (1 << 15),    // Makes the peep feel sick (e.g. after an extreme ride)
    PEEP_FLAGS_PURPLE = (1 << 16),    // Makes surrounding peeps purple
    PEEP_FLAGS_PIZZA = (1 << 17),     // Gives passing peeps pizza
    PEEP_FLAGS_EXPLODE = (1 << 18),
    PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE = (1 << 19),
    PEEP_FLAGS_PARK_ENTRANCE_CHOSEN = (1 << 20), // Set when the nearest park entrance has been chosen
    PEEP_FLAGS_21 = (1 << 21),
    PEEP_FLAGS_CONTAGIOUS = (1 << 22), // Makes any peeps in surrounding tiles sick
    PEEP_FLAGS_JOY = (1 << 23),        // Makes the peep jump in joy
    PEEP_FLAGS_ANGRY = (1 << 24),
    PEEP_FLAGS_ICE_CREAM = (1 << 25),            // Gives passing peeps ice cream and they wave back
    PEEP_FLAGS_NICE_RIDE_DEPRECATED = (1 << 26), // Used to make the peep think "Nice ride! But not as good as the Phoenix..."
                                                 // on exiting a ride
    PEEP_FLAGS_INTAMIN_DEPRECATED = (1 << 27),   // Used to make the peep think "I'm so excited - It's an Intamin ride!" while
                                                 // riding on a Intamin ride.
    PEEP_FLAGS_HERE_WE_ARE = (1 << 28),          // Makes the peep think  "...and here we are on X!" while riding a ride
    PEEP_FLAGS_TWITCH_DEPRECATED = (1u << 31),   // Formerly used for twitch integration
};

enum PeepNextFlags
{
    PEEP_NEXT_FLAG_DIRECTION_MASK = 0x3,
    PEEP_NEXT_FLAG_IS_SLOPED = (1 << 2),
    PEEP_NEXT_FLAG_IS_SURFACE = (1 << 3),
    PEEP_NEXT_FLAG_UNUSED = (1 << 4),
};

enum class PeepSpriteType : uint8_t
{
    Normal = 0,
    Handyman = 1,
    Mechanic = 2,
    Security = 3,
    EntertainerPanda = 4,
    EntertainerTiger = 5,
    EntertainerElephant = 6,
    EntertainerRoman = 7,
    EntertainerGorilla = 8,
    EntertainerSnowman = 9,
    EntertainerKnight = 10,
    EntertainerAstronaut = 11,
    EntertainerBandit = 12,
    EntertainerSheriff = 13,
    EntertainerPirate = 14,
    IceCream = 15,
    Chips = 16,
    Burger = 17,
    Drink = 18,
    Balloon = 19,
    Candyfloss = 20,
    Umbrella = 21,
    Pizza = 22,
    SecurityAlt = 23,
    Popcorn = 24,
    ArmsCrossed = 25,
    HeadDown = 26,
    Nauseous = 27,
    VeryNauseous = 28,
    RequireToilet = 29,
    Hat = 30,
    HotDog = 31,
    Tentacle = 32,
    ToffeeApple = 33,
    Doughnut = 34,
    Coffee = 35,
    Chicken = 36,
    Lemonade = 37,
    Watching = 38,
    Pretzel = 39,
    Sunglasses = 40,
    SuJongkwa = 41,
    Juice = 42,
    FunnelCake = 43,
    Noodles = 44,
    Sausage = 45,
    Soup = 46,
    Sandwich = 47,
    Count = 48,

    Invalid = 255
};

// Flags used by peep->WindowInvalidateFlags
enum PeepInvalidate
{
    PEEP_INVALIDATE_PEEP_THOUGHTS = 1,
    PEEP_INVALIDATE_PEEP_STATS = 1 << 1,
    PEEP_INVALIDATE_PEEP_2 = 1 << 2,
    PEEP_INVALIDATE_PEEP_INVENTORY = 1 << 3,
    PEEP_INVALIDATE_STAFF_STATS = 1 << 4,
    PEEP_INVALIDATE_PEEP_ACTION = 1 << 5, // Currently set only when GuestHeadingToRideId is changed
};

struct Guest;
struct Staff;

struct Peep : EntityBase
{
    char* Name;
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
    RideId CurrentRide;
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
    RideId InteractionRideIndex;
    uint32_t Id;
    uint8_t PathCheckOptimisation; // see peep.checkForPath
    TileCoordsXYZD PathfindGoal;
    std::array<TileCoordsXYZD, 4> PathfindHistory;
    uint8_t WalkingFrameNum;
    uint32_t PeepFlags;

public: // Peep
    void Update();
    std::optional<CoordsXY> UpdateAction(int16_t& xy_distance);
    std::optional<CoordsXY> UpdateAction();
    void SetState(PeepState new_state);
    void Remove();
    void UpdateCurrentActionSpriteType();
    void SwitchToSpecialSprite(uint8_t special_sprite_id);
    void StateReset();
    [[nodiscard]] uint8_t GetNextDirection() const;
    bool GetNextIsSloped() const;
    bool GetNextIsSurface() const;
    void SetNextFlags(uint8_t next_direction, bool is_sloped, bool is_surface);
    bool CanBePickedUp() const;
    void Pickup();
    void PickupAbort(int32_t old_x);
    [[nodiscard]] GameActions::Result Place(const TileCoordsXYZ& location, bool apply);
    void RemoveFromRide();
    void FormatActionTo(Formatter&) const;
    void FormatNameTo(Formatter&) const;
    [[nodiscard]] std::string GetName() const;
    bool SetName(std::string_view value);
    bool IsActionWalking() const;
    bool IsActionIdle() const;
    bool IsActionInterruptable() const;

    // Reset the peep's stored goal, which means they will forget any stored pathfinding history
    // on the next GuestPathfinding::ChooseDirection call.
    void ResetPathfindGoal();

    void SetDestination(const CoordsXY& coords);
    void SetDestination(const CoordsXY& coords, int32_t tolerance);
    [[nodiscard]] CoordsXY GetDestination() const;

    void Serialise(class DataSerialiser& stream);
    void Paint(paint_session& session, int32_t imageDirection) const;

    // TODO: Make these private again when done refactoring
public: // Peep
    [[nodiscard]] bool CheckForPath();
    bool PathIsBlockedByVehicle();
    bool IsOnLevelCrossing();
    void PerformNextAction(uint8_t& pathing_result);
    void PerformNextAction(uint8_t& pathing_result, TileElement*& tile_result);
    [[nodiscard]] int32_t GetZOnSlope(int32_t tile_x, int32_t tile_y);
    void SwitchNextActionSpriteType();
    [[nodiscard]] PeepActionSpriteType GetActionSpriteType();

private:
    void UpdateFalling();
    void Update1();
    void UpdatePicked();
};

struct rct_sprite_bounds
{
    uint8_t sprite_width;           // 0x00
    uint8_t sprite_height_negative; // 0x01
    uint8_t sprite_height_positive; // 0x02
};

struct rct_peep_animation
{
    uint32_t base_image; // 0x00
    size_t num_frames;
    const uint8_t* frame_offsets;
};

struct rct_peep_animation_entry
{
    const rct_peep_animation* sprite_animation; // 0x00
    const rct_sprite_bounds* sprite_bounds;     // 0x04
};

enum
{
    PATHING_DESTINATION_REACHED = 1 << 0,
    PATHING_OUTSIDE_PARK = 1 << 1,
    PATHING_RIDE_EXIT = 1 << 2,
    PATHING_RIDE_ENTRANCE = 1 << 3,
};

// rct2: 0x00982708
extern const rct_peep_animation_entry g_peep_animation_entries[EnumValue(PeepSpriteType::Count)];
extern const bool gSpriteTypeToSlowWalkMap[48];

extern uint8_t gPeepWarningThrottle[16];

int32_t peep_get_staff_count();
void peep_update_all();
void peep_problem_warnings_update();
void peep_stop_crowd_noise();
void peep_update_crowd_noise();
void peep_update_days_in_queue();
void peep_applause();
int32_t get_peep_face_sprite_small(Guest* peep);
int32_t get_peep_face_sprite_large(Guest* peep);
void peep_sprite_remove(Peep* peep);

void peep_window_state_update(Peep* peep);
void peep_decrement_num_riders(Peep* peep);

void peep_set_map_tooltip(Peep* peep);
int32_t peep_compare(const EntityId sprite_index_a, const EntityId sprite_index_b);

void peep_update_names(bool realNames);

StringId get_real_name_string_id_from_id(uint32_t id);

inline const rct_peep_animation& GetPeepAnimation(
    PeepSpriteType spriteType, PeepActionSpriteType actionSpriteType = PeepActionSpriteType::None)
{
    return g_peep_animation_entries[EnumValue(spriteType)].sprite_animation[EnumValue(actionSpriteType)];
};

inline const rct_sprite_bounds& GetSpriteBounds(
    PeepSpriteType spriteType, PeepActionSpriteType actionSpriteType = PeepActionSpriteType::None)
{
    return g_peep_animation_entries[EnumValue(spriteType)].sprite_bounds[EnumValue(actionSpriteType)];
};
