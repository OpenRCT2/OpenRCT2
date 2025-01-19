/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../entity/EntityBase.h"
#include "../localisation/StringIdType.h"
#include "../ride/RideTypes.h"
#include "../ride/Station.h"
#include "../world/Location.hpp"

#include <array>
#include <optional>
#include <string>
#include <string_view>

constexpr uint8_t kPeepMinEnergy = 32;
constexpr uint8_t kPeepMaxEnergy = 128;
constexpr uint8_t kPeepMaxEnergyTarget = 255; // Oddly, this differs from max energy!

constexpr auto kPeepClearanceHeight = 4 * kCoordsZStep;

class Formatter;
struct TileElement;
struct PaintSession;

namespace OpenRCT2::GameActions
{
    class Result;
}

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
    StaffCheckBoard = 14,
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

enum class PeepAnimationType : uint8_t
{
    Walking = 0,
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
    Hanging = 11,
    StaffMower = 12,
    Wow = 13,
    ThrowUp = 14,
    Jump = 15,
    StaffSweep = 16,
    Drowning = 17,
    StaffAnswerCall = 18,
    StaffAnswerCall2 = 19,
    StaffCheckBoard = 20,
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
    PEEP_FLAGS_DEBUG_PATHFINDING = (1 << 2), // Enables debug logging for path finding
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
    PEEP_FLAGS_POSITION_FROZEN = (1 << 29),      // Prevents the peep from moving around, thus keeping them in place
    PEEP_FLAGS_ANIMATION_FROZEN = (1 << 30),     // Prevents the peep sprite from updating
    PEEP_FLAGS_TWITCH_DEPRECATED = (1u << 31),   // Formerly used for twitch integration
};

enum PeepNextFlags
{
    PEEP_NEXT_FLAG_DIRECTION_MASK = 0x3,
    PEEP_NEXT_FLAG_IS_SLOPED = (1 << 2),
    PEEP_NEXT_FLAG_IS_SURFACE = (1 << 3),
    PEEP_NEXT_FLAG_UNUSED = (1 << 4),
};

enum class PeepAnimationGroup : uint8_t
{
    Normal = 0,

    // Security staff
    Alternate = 1,

    // Guest variations
    IceCream = 1,
    Chips = 2,
    Burger = 3,
    Drink = 4,
    Balloon = 5,
    Candyfloss = 6,
    Umbrella = 7,
    Pizza = 8,
    Popcorn = 9,
    ArmsCrossed = 10,
    HeadDown = 11,
    Nauseous = 12,
    VeryNauseous = 13,
    RequireToilet = 14,
    Hat = 15,
    HotDog = 16,
    Tentacle = 17,
    ToffeeApple = 18,
    Doughnut = 19,
    Coffee = 20,
    Chicken = 21,
    Lemonade = 22,
    Watching = 23,
    Pretzel = 24,
    Sunglasses = 25,
    Sujeonggwa = 26,
    Juice = 27,
    FunnelCake = 28,
    Noodles = 29,
    Sausage = 30,
    Soup = 31,
    Sandwich = 32,

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
    ObjectEntryIndex AnimationObjectIndex;
    PeepAnimationGroup AnimationGroup;
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
    PeepAnimationType AnimationType;
    // Seems to be used like a local variable, as it's always set before calling SwitchNextAnimationType, which
    // reads this again
    PeepAnimationType NextAnimationType;
    uint8_t AnimationImageIdOffset;
    PeepActionType Action;
    uint8_t AnimationFrameNum;
    uint8_t StepProgress;
    union
    {
        uint8_t MazeLastEdge;
        ::Direction PeepDirection; // Direction ?
    };
    RideId InteractionRideIndex;
    uint32_t PeepId;
    uint8_t PathCheckOptimisation; // see peep.checkForPath
    TileCoordsXYZD PathfindGoal;
    std::array<TileCoordsXYZD, 4> PathfindHistory;
    uint8_t WalkingAnimationFrameNum;
    uint32_t PeepFlags;

public: // Peep
    void Update();
    std::optional<CoordsXY> UpdateAction(int16_t& xy_distance);
    std::optional<CoordsXY> UpdateAction();
    bool UpdateActionAnimation();
    std::optional<CoordsXY> UpdateWalkingAction(const CoordsXY& differenceLoc, int16_t& xy_distance);
    void UpdateWalkingAnimation();
    void ThrowUp();
    void SetState(PeepState new_state);
    void Remove();
    void UpdateCurrentAnimationType();
    void UpdateSpriteBoundingBox();
    void SwitchToSpecialSprite(uint8_t special_sprite_id);
    void StateReset();
    [[nodiscard]] uint8_t GetNextDirection() const;
    bool GetNextIsSloped() const;
    bool GetNextIsSurface() const;
    void SetNextFlags(uint8_t next_direction, bool is_sloped, bool is_surface);
    bool CanBePickedUp() const;
    void Pickup();
    void PickupAbort(int32_t old_x);
    [[nodiscard]] OpenRCT2::GameActions::Result Place(const TileCoordsXYZ& location, bool apply);
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
    void Paint(PaintSession& session, int32_t imageDirection) const;

    // TODO: Make these private again when done refactoring
public: // Peep
    [[nodiscard]] bool CheckForPath();
    bool ShouldWaitForLevelCrossing();
    bool IsOnLevelCrossing();
    bool IsOnPathBlockedByVehicle();
    void PerformNextAction(uint8_t& pathing_result);
    void PerformNextAction(uint8_t& pathing_result, TileElement*& tile_result);
    [[nodiscard]] int32_t GetZOnSlope(int32_t tile_x, int32_t tile_y);
    void SwitchNextAnimationType();
    [[nodiscard]] PeepAnimationType GetAnimationType();

private:
    void UpdateFalling();
    void Update1();
    void UpdatePicked();
};

enum
{
    PATHING_DESTINATION_REACHED = 1 << 0,
    PATHING_OUTSIDE_PARK = 1 << 1,
    PATHING_RIDE_EXIT = 1 << 2,
    PATHING_RIDE_ENTRANCE = 1 << 3,
};

int32_t PeepGetStaffCount();
void PeepUpdateAll();
void PeepUpdateAllBoundingBoxes();
void PeepProblemWarningsUpdate();
void PeepStopCrowdNoise();
void PeepUpdateCrowdNoise();
void PeepUpdateDaysInQueue();
void PeepApplause();
int32_t GetPeepFaceSpriteSmall(Guest* peep);
int32_t GetPeepFaceSpriteLarge(Guest* peep);
void PeepEntityRemove(Peep* peep);

void PeepWindowStateUpdate(Peep* peep);
void PeepDecrementNumRiders(Peep* peep);

void PeepSetMapTooltip(Peep* peep);
int32_t PeepCompare(const EntityId sprite_index_a, const EntityId sprite_index_b);

void PeepUpdateNames();

StringId GetRealNameStringIDFromPeepID(uint32_t id);
