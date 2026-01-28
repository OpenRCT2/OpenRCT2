/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <utility>

constexpr uint8_t kPeepMinEnergy = 32;
constexpr uint8_t kPeepMaxEnergy = 128;
constexpr uint8_t kPeepMaxEnergyTarget = 255; // Oddly, this differs from max energy!

constexpr auto kPeepClearanceHeight = 4 * kCoordsZStep;

struct PaintSession;

namespace OpenRCT2
{
    class Formatter;
    struct TileElement;
} // namespace OpenRCT2

namespace OpenRCT2::GameActions
{
    class Result;
}

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
}

enum class PeepState : uint8_t
{
    falling = 0, // Drowning is part of falling
    one = 1,     // was PEEP_STATE_1
    queuingFront = 2,
    onRide = 3,
    leavingRide = 4,
    walking = 5,
    queuing = 6,
    enteringRide = 7,
    sitting = 8,
    picked = 9,
    patrolling = 10, // Not sure
    mowing = 11,
    sweeping = 12,
    enteringPark = 13,
    leavingPark = 14,
    answering = 15,
    fixing = 16,
    buying = 17,
    watching = 18,
    emptyingBin = 19,
    usingBin = 20,
    watering = 21,
    headingToInspection = 22,
    inspecting = 23,
};

enum class PeepSittingSubState : uint8_t
{
    tryingToSit, // was = 0
    satDown      // was unassigned
};

enum class PeepRideSubState : uint8_t
{
    atEntrance = 0,
    inEntrance = 1,
    freeVehicleCheck = 2, // Spend money on ride
    leaveEntrance = 3,    // Calculate what direction and where to go after committing to entering vehicle
    approachVehicle = 4,
    enterVehicle = 5,
    onRide = 6,
    leaveVehicle = 7,
    approachExit = 8,
    inExit = 9,
    inQueue = 10,
    atQueueFront = 11,
    approachVehicleWaypoints = 12,
    approachExitWaypoints = 13,
    approachSpiralSlide = 14,
    onSpiralSlide = 15,
    leaveSpiralSlide = 16,
    mazePathfinding = 17,
    leaveExit = 18,
    approachShop = 19,
    interactShop = 20,
    leaveShop = 21,
};

// Substates for guests that are inside the Spiral Slide. Declared as uint16_t to match DestinationX.
enum class PeepSpiralSlideSubState : uint16_t
{
    goingUp = 0,
    prepareToSlide = 1,
    slidingDown = 2,
    finishedSliding = 3
};

enum class PeepUsingBinSubState : uint8_t
{
    walkingToBin = 0,
    goingBack,
};

enum class PeepActionType : uint8_t
{
    checkTime = 0,
    // If no food then check watch
    eatFood = 1,
    shakeHead = 2,
    emptyPockets = 3,
    sittingEatFood = 4,
    sittingCheckWatch = 4,
    sittingLookAroundLeft = 5,
    sittingLookAroundRight = 6,
    wow = 7,
    throwUp = 8,
    jump = 9,
    staffSweep = 10,
    drowning = 11,
    staffAnswerCall = 12,
    staffAnswerCall2 = 13,
    staffCheckBoard = 14,
    staffFix = 15,
    staffFix2 = 16,
    staffFixGround = 17,
    staffFix3 = 18,
    staffWatering = 19,
    joy = 20,
    readMap = 21,
    wave = 22,
    staffEmptyBin = 23,
    wave2 = 24,
    takePhoto = 25,
    clap = 26,
    disgust = 27,
    drawPicture = 28,
    beingWatched = 29,
    withdrawMoney = 30,

    idle = 254,
    walking = 255,
};

enum class PeepAnimationType : uint8_t
{
    walking = 0,
    checkTime = 1,
    watchRide = 2,
    eatFood = 3,
    shakeHead = 4,
    emptyPockets = 5,
    holdMat = 6,
    sittingIdle = 7,
    sittingEatFood = 8,
    sittingLookAroundLeft = 9,
    sittingLookAroundRight = 10,
    hanging = 11,
    staffMower = 12,
    wow = 13,
    throwUp = 14,
    jump = 15,
    staffSweep = 16,
    drowning = 17,
    staffAnswerCall = 18,
    staffAnswerCall2 = 19,
    staffCheckBoard = 20,
    staffFix = 21,
    staffFix2 = 22,
    staffFixGround = 23,
    staffFix3 = 24,
    staffWatering = 25,
    joy = 26,
    readMap = 27,
    wave = 28,
    staffEmptyBin = 29,
    wave2 = 30,
    takePhoto = 31,
    clap = 32,
    disgust = 33,
    drawPicture = 34,
    beingWatched = 35,
    withdrawMoney = 36,

    invalid = 255
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
    normal = 0,

    // Security staff
    alternate = 1,

    // Guest variations
    iceCream = 1,
    chips = 2,
    burger = 3,
    drink = 4,
    balloon = 5,
    candyfloss = 6,
    umbrella = 7,
    pizza = 8,
    popcorn = 9,
    armsCrossed = 10,
    headDown = 11,
    nauseous = 12,
    veryNauseous = 13,
    requireToilet = 14,
    hat = 15,
    hotDog = 16,
    tentacle = 17,
    toffeeApple = 18,
    doughnut = 19,
    coffee = 20,
    chicken = 21,
    lemonade = 22,
    watching = 23,
    pretzel = 24,
    sunglasses = 25,
    sujeonggwa = 26,
    juice = 27,
    funnelCake = 28,
    noodles = 29,
    sausage = 30,
    soup = 31,
    sandwich = 32,

    invalid = 255
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
    OpenRCT2::ObjectEntryIndex AnimationObjectIndex;
    PeepAnimationGroup AnimationGroup;
    OpenRCT2::Drawing::Colour TshirtColour;
    OpenRCT2::Drawing::Colour TrousersColour;
    union
    {
        uint16_t DestinationX;
        PeepSpiralSlideSubState spiralSlideSubstate;
    };
    union
    {
        uint16_t DestinationY;
        uint16_t spiralSlideGoingUpTimer;
    };
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
        uint8_t timesSlidDown;
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
    std::optional<CoordsXY> UpdateAction(int16_t& xy_distance);
    std::optional<CoordsXY> UpdateAction();
    bool UpdateActionAnimation();
    std::optional<CoordsXY> UpdateWalkingAction(const CoordsXY& differenceLoc, int16_t& xy_distance);
    void UpdateWalkingAnimation();
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
    void FormatActionTo(OpenRCT2::Formatter&) const;
    void FormatNameTo(OpenRCT2::Formatter&) const;
    [[nodiscard]] std::string GetName() const;
    bool SetName(std::string_view value);
    bool IsActionWalking() const;
    bool IsActionIdle() const;
    bool IsActionInterruptable() const;
    bool IsActionInterruptableSafely() const;

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
    std::pair<uint8_t, OpenRCT2::TileElement*> PerformNextAction();
    [[nodiscard]] int32_t GetZOnSlope(int32_t tile_x, int32_t tile_y);
    void SwitchNextAnimationType();
    [[nodiscard]] PeepAnimationType GetAnimationType();

protected:
    bool ShouldWaitForLevelCrossing() const;
    bool IsOnLevelCrossing() const;
    bool IsOnPathBlockedByVehicle() const;
    void UpdateWaitingAtCrossing();
    void UpdateFalling();
    void Update1();
    void UpdatePicked();
    uint32_t GetStepsToTake() const;
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
int32_t PeepCompare(EntityId sprite_index_a, EntityId sprite_index_b);

void PeepUpdateNames();

StringId GetRealNameStringIDFromPeepID(uint32_t id);
