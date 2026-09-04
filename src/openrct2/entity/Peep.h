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
#include "../core/FlagHolder.hpp"
#include "../entity/EntityBase.h"
#include "../localisation/StringIdType.h"
#include "../object/ObjectTypes.h"
#include "../world/Location.hpp"

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace OpenRCT2::GameActions
{
    class Result;
}

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
}

namespace OpenRCT2
{
    class Formatter;
    struct TileElement;

    constexpr uint8_t kPeepMinEnergy = 32;
    constexpr uint8_t kPeepMaxEnergy = 128;
    constexpr uint8_t kPeepMaxEnergyTarget = 255; // Oddly, this differs from max energy!

    constexpr auto kPeepClearanceHeight = 4 * kCoordsZStep;

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

    enum class PeepFlag : uint32_t
    {
        leavingPark = 0,
        slowWalk = 1,
        debugPathfinding = 2, // Enables debug logging for path finding
        tracking = 3,
        waving = 4,              // Makes the peep wave
        hasPaidForParkEntry = 5, // Set on paying to enter park
        photo = 6,               // Makes the peep take a picture
        painting = 7,
        wow = 8,        // Makes a peep WOW2
        litter = 9,     // Makes the peep throw litter
        lost = 10,      // Makes the peep feel lost (animation triggered)
        hunger = 11,    // Makes the peep become hungry quicker
        toilet = 12,    // Makes the peep want to go to the toilet
        crowded = 13,   // The peep will start feeling crowded
        happiness = 14, // The peep will start increasing happiness
        nausea = 15,    // Makes the peep feel sick (e.g. after an extreme ride)
        purple = 16,    // Makes surrounding peeps purple
        pizza = 17,     // Gives passing peeps pizza
        explode = 18,
        rideShouldBeMarkedAsFavourite = 19,
        parkEntranceChosen = 20, // Set when the nearest park entrance has been chosen
        unknown21 = 21,
        contagious = 22, // Makes any peeps in surrounding tiles sick
        joy = 23,        // Makes the peep jump in joy
        angry = 24,
        iceCream = 25,           // Gives passing peeps ice cream and they wave back
        niceRideDeprecated = 26, // Used to make the peep think "Nice ride! But not as good as the
                                 // Phoenix..." on exiting a ride
        intaminDeprecated = 27,  // Used to make the peep think "I'm so excited - It's an Intamin ride!" while
                                 // riding on a Intamin ride.
        hereWeAre = 28,          // Makes the peep think  "...and here we are on X!" while riding a ride
        positionFrozen = 29,     // Prevents the peep from moving around, thus keeping them in place
        animationFrozen = 30,    // Prevents the peep sprite from updating
        twitchDeprecated = 31,   // Formerly used for twitch integration
    };
    using PeepFlags = FlagHolder<uint32_t, PeepFlag>;

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
        PEEP_INVALIDATE_PEEP_ACTION = 1 << 5, // Currently set only when guestHeadingToRideId is changed
    };

    enum class PeepActionDescriptionType : uint8_t
    {
        walking,
        sitting,
        watchingScenery,
        drowning,
        pickedUp,
        headingFor,
        watchingRide,
        watchingRideConstruction,
        watchingRideConstructionUnspecific,
        queuingFor,
        onRide,
        inRide,
        atShop,
        leavingPark,
        sweepingFootpath,
        emptyingBin,
        wateringGardens,
        mowingGrass,
        headingToInspectRide,
        inspectingRide,
        fixingRide,
        answeringRadioCall,
        respondingToBreakdownCall,

        count,
    };
    constexpr size_t kNumPeepActionDescriptionTypes = static_cast<size_t>(PeepActionDescriptionType::count);

    struct PeepActionDescription
    {
        PeepActionDescriptionType type;
        RideId rideId = RideId::GetNull();
    };

    struct Guest;

    struct Peep : EntityBase
    {
        char* name;
        CoordsXYZ nextLoc;
        uint8_t nextFlags;
        PeepState state;
        union
        {
            uint8_t subState;
            PeepSittingSubState sittingSubState;
            PeepRideSubState rideSubState;
            PeepUsingBinSubState usingBinSubState;
        };
        ObjectEntryIndex animationObjectIndex;
        PeepAnimationGroup animationGroup;
        Drawing::Colour tShirtColour;
        Drawing::Colour trousersColour;
        union
        {
            uint16_t destinationX;
            PeepSpiralSlideSubState spiralSlideSubstate;
        };
        union
        {
            uint16_t destinationY;
            uint16_t spiralSlideGoingUpTimer;
        };
        uint8_t destinationTolerance; // How close to destination before next action/state 0 = exact
        uint8_t var37;
        uint8_t energy;
        uint8_t energyTarget;
        uint8_t mass;
        uint8_t windowInvalidateFlags;
        RideId currentRide;
        StationIndex currentRideStation;
        uint8_t currentTrain;
        union
        {
            struct
            {
                uint8_t currentCar;
                uint8_t currentSeat;
            };
            uint16_t timeToSitdown;
            struct
            {
                uint8_t timeToStand;
                uint8_t standingFlags;
            };
            uint8_t timesSlidDown;
        };
        // Normally 0, 1 for carrying sliding board on spiral slide ride, 2 for carrying lawn mower
        uint8_t specialSprite;
        PeepAnimationType animationType;
        // Seems to be used like a local variable, as it's always set before calling SwitchNextAnimationType, which
        // reads this again
        PeepAnimationType nextAnimationType;
        uint8_t animationImageIdOffset;
        PeepActionType action;
        uint8_t animationFrameNum;
        uint8_t stepProgress;
        union
        {
            uint8_t mazeLastEdge;
            ::Direction peepDirection; // Direction ?
        };
        RideId interactionRideIndex;
        uint32_t peepId;
        uint8_t pathCheckOptimisation; // see peep.checkForPath
        TileCoordsXYZD pathfindGoal;
        std::array<TileCoordsXYZD, 4> pathfindHistory;
        uint8_t walkingAnimationFrameNum;
        PeepFlags peepFlags;

    public: // Peep
        std::optional<CoordsXY> updateAction(int16_t& xy_distance);
        std::optional<CoordsXY> updateAction();
        bool updateActionAnimation();
        std::optional<CoordsXY> updateWalkingAction(const CoordsXY& differenceLoc, int16_t& xy_distance);
        void updateWalkingAnimation();
        void setState(PeepState new_state);
        void remove();
        void updateCurrentAnimationType();
        void updateSpriteBoundingBox();
        void switchToSpecialSprite(uint8_t special_sprite_id);
        void stateReset();
        [[nodiscard]] uint8_t getNextDirection() const;
        bool getNextIsSloped() const;
        bool getNextIsSurface() const;
        void setNextFlags(uint8_t next_direction, bool is_sloped, bool is_surface);
        bool canBePickedUp() const;
        void pickup();
        void pickupAbort(int32_t old_x);
        [[nodiscard]] GameActions::Result place(const TileCoordsXYZ& location, bool apply);
        void removeFromRide();
        PeepActionDescription getActionDescription() const;
        void formatNameTo(Formatter&) const;
        [[nodiscard]] std::string getName() const;
        bool setName(std::string_view value);
        bool isActionWalking() const;
        bool isActionIdle() const;
        bool isActionInterruptable() const;
        bool isActionInterruptableSafely() const;

        // Reset the peep's stored goal, which means they will forget any stored pathfinding history
        // on the next GuestPathfinding::ChooseDirection call.
        void resetPathfindGoal();

        void setDestination(const CoordsXY& coords);
        void setDestination(const CoordsXY& coords, int32_t tolerance);
        [[nodiscard]] CoordsXY getDestination() const;

        void serialise(class DataSerialiser& stream);

        // TODO: Make these private again when done refactoring
    public: // Peep
        [[nodiscard]] bool checkForPath();
        std::pair<uint8_t, TileElement*> performNextAction();
        [[nodiscard]] int32_t getZOnSlope(int32_t tile_x, int32_t tile_y);
        void switchNextAnimationType();
        [[nodiscard]] PeepAnimationType getAnimationType();

    protected:
        bool shouldWaitForLevelCrossing() const;
        bool isOnLevelCrossing() const;
        bool isOnPathBlockedByVehicle() const;
        void updateWaitingAtCrossing();
        void updateFalling();
        void update1();
        void updatePicked();
        uint32_t getStepsToTake() const;
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
} // namespace OpenRCT2
