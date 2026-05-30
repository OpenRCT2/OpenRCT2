/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/BitSet.hpp"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/ShopItem.h"
#include "Peep.h"

struct CarEntry;

namespace OpenRCT2
{
    constexpr int8_t kPeepMaxThoughts = 5;

    constexpr int8_t kPeepHungerWarningThreshold = 25;
    constexpr int8_t kPeepThirstWarningThreshold = 25;
    constexpr int8_t kPeepToiletWarningThreshold = 28;
    constexpr int8_t kPeepLitterWarningThreshold = 23;
    constexpr int8_t kPeepDisgustWarningThreshold = 22;
    constexpr int8_t kPeepVandalismWarningThreshold = 15;
    constexpr int8_t kPeepNoExitWarningThreshold = 8;
    constexpr int8_t kPeepLostWarningThreshold = 8;
    constexpr int8_t kPeepTooLongQueueThreshold = 25;

    constexpr int kPeepMaxHappiness = 255;
    constexpr int kPeepMaxHunger = 255;
    constexpr int16_t kPeepMaxToilet = 255;
    constexpr int16_t kPeepMaxNausea = 255;
    constexpr int kPeepMaxThirst = 255;

    enum class PeepThoughtType : uint8_t
    {
        cantAffordRide = 0,   // "I can't afford"
        spentMoney = 1,       // "I've spent all my money"
        sick = 2,             // "I feel sick"
        verySick = 3,         // "I feel very sick"
        moreThrilling = 4,    // "I want to go on something more thrilling than X"
        intense = 5,          // "X looks too intense for me"
        haventFinished = 6,   // "I haven't finished my X yet"
        sickening = 7,        // "Just looking at X makes me feel sick"
        badValue = 8,         // "I'm not paying that much to go on X"
        goHome = 9,           // "I want to go home"
        goodValue = 10,       // "X is really good value"
        alreadyGot = 11,      // "I've already got"
        cantAffordItem = 12,  // "I can't afford"
        notHungry = 13,       // "I'm not hungry"
        notThirsty = 14,      // "I'm not thirsty"
        drowning = 15,        // "Help! I'm drowning!"
        lost = 16,            // "I'm lost!"
        wasGreat = 17,        // "X was great"
        queuingAges = 18,     // "I've been queuing for X for ages"
        tired = 19,           // "I'm tired"
        hungry = 20,          // "I'm hungry"
        thirsty = 21,         // "I'm thirsty"
        toilet = 22,          // "I need to go to the toilet"
        cantFind = 23,        // "I can't find X"
        notPaying = 24,       // "I'm not paying that much to use X"
        notWhileRaining = 25, // "I'm not going on X while it's raining"
        badLitter = 26,       // "The litter here is really bad"
        cantFindExit = 27,    // "I can't find the exit"
        getOff = 28,          // "I want to get off X"
        getOut = 29,          // "I want to get out of X"
        notSafe = 30,         // "I'm not going on X - it isn't safe"
        pathDisgusting = 31,  // "This path is disgusting"
        crowded = 32,         // "It's too crowded here"
        vandalism = 33,       // "The vandalism here is really bad"
        scenery = 34,         // "Great scenery!"
        veryClean = 35,       // "This park is very clean and tidy"
        fountains = 36,       // "The jumping fountains are great"
        music = 37,           // "The music is nice here"
        balloon = 38,         // "This balloon from X is really good value"
        toy = 39,             // "This cuddly toy from X is really good value"
        map = 40,
        photo = 41, // "This on ride photo from X is really good value"
        umbrella = 42,
        drink = 43,
        burger = 44,
        chips = 45,
        iceCream = 46,
        candyfloss = 47,

        pizza = 51,

        popcorn = 53,
        hotDog = 54,
        tentacle = 55,
        hat = 56,
        toffeeApple = 57,
        tshirt = 58,
        doughnut = 59,
        coffee = 60,

        chicken = 62,
        lemonade = 63,

        wow = 67, // "Wow!"

        wow2 = 70,        // "Wow!"
        watched = 71,     // "I have the strangest feeling someone is watching me"
        balloonMuch = 72, // "I'm not paying that much to get a balloon from X"
        toyMuch = 73,
        mapMuch = 74,
        photoMuch = 75,
        umbrellaMuch = 76,
        drinkMuch = 77,
        burgerMuch = 78,
        chipsMuch = 79,
        iceCreamMuch = 80,
        candyflossMuch = 81,

        pizzaMuch = 85,

        popcornMuch = 87,
        hotDogMuch = 88,
        tentacleMuch = 89,
        hatMuch = 90,
        toffeeAppleMuch = 91,
        tshirtMuch = 92,
        doughnutMuch = 93,
        coffeeMuch = 94,

        chickenMuch = 96,
        lemonadeMuch = 97,

        photo2 = 104, // "This on-ride photo from X is really good value"
        photo3 = 105,
        photo4 = 106,
        pretzel = 107,
        hotChocolate = 108,
        icedTea = 109,
        funnelCake = 110,
        sunglasses = 111,
        beefNoodles = 112,
        friedRiceNoodles = 113,
        wontonSoup = 114,
        meatballSoup = 115,
        fruitJuice = 116,
        soybeanMilk = 117,
        sujongkwa = 118,
        subSandwich = 119,
        cookie = 120,

        roastSausage = 124,

        photo2Much = 136,
        photo3Much = 137,
        photo4Much = 138,
        pretzelMuch = 139,
        hotChocolateMuch = 140,
        icedTeaMuch = 141,
        funnelCakeMuch = 142,
        sunglassesMuch = 143,
        beefNoodlesMuch = 144,
        friedRiceNoodlesMuch = 145,
        wontonSoupMuch = 146,
        meatballSoupMuch = 147,
        fruitJuiceMuch = 148,
        soybeanMilkMuch = 149,
        sujongkwaMuch = 150,
        subSandwichMuch = 151,
        cookieMuch = 152,

        roastSausageMuch = 156,

        help = 168,               // "Help! Put me down!"
        runningOut = 169,         // "I'm running out of cash!"
        newRide = 170,            // "Wow! A new ride being built!"
        niceRideDeprecated = 171, // "Nice ride! But not as good as the Phoenix..."
        excitedDeprecated = 172,  // "I'm so excited - It's an Intamin ride!"
        hereWeAre = 173,          // "...and here we are on X!"

        none = 255
    };

    enum class PeepNauseaTolerance : uint8_t
    {
        none,
        low,
        average,
        high
    };

    static constexpr uint16_t kPeepThoughtItemNone = std::numeric_limits<uint16_t>::max();

    struct PeepThought
    {
        PeepThoughtType type;
        union
        {
            RideId rideId;
            ShopItem shopItem;
            uint16_t item;
        };
        uint8_t freshness;     // larger is less fresh
        uint8_t fresh_timeout; // updates every tick
    };

    struct Guest;
    struct Staff;

    struct IntensityRange
    {
    private:
        uint8_t _value{};

    public:
        explicit IntensityRange(uint8_t value)
            : _value(value)
        {
        }

        IntensityRange(uint8_t min, uint8_t max)
            : _value(std::min<uint8_t>(min, 15) | (std::min<uint8_t>(max, 15) << 4))
        {
        }

        uint8_t GetMinimum() const
        {
            return _value & 0x0F;
        }

        uint8_t GetMaximum() const
        {
            return _value >> 4;
        }

        IntensityRange WithMinimum(uint8_t value) const
        {
            return IntensityRange(value, GetMaximum());
        }

        IntensityRange WithMaximum(uint8_t value) const
        {
            return IntensityRange(GetMinimum(), value);
        }

        explicit operator uint8_t() const
        {
            return _value;
        }

        friend bool operator==(const IntensityRange& lhs, const IntensityRange& rhs)
        {
            return lhs._value == rhs._value;
        }

        friend bool operator!=(const IntensityRange& lhs, const IntensityRange& rhs)
        {
            return lhs._value != rhs._value;
        }
    };

    struct Guest : Peep
    {
        static constexpr auto cEntityType = EntityType::guest;

    public:
        uint8_t guestNumRides;
        EntityId guestNextInQueue;
        int32_t parkEntryTime;
        RideId guestHeadingToRideId;
        uint8_t guestIsLostCountdown;
        uint8_t guestTimeOnRide;
        money64 paidToEnter;
        money64 paidOnRides;
        money64 paidOnFood;
        money64 paidOnDrink;
        money64 paidOnSouvenirs;
        bool outsideOfPark;
        uint8_t happiness;
        uint8_t happinessTarget;
        uint8_t nausea;
        uint8_t nauseaTarget;
        uint8_t hunger;
        uint8_t thirst;
        uint8_t toilet;
        uint8_t timeToConsume;
        IntensityRange intensity{ 0 };
        PeepNauseaTolerance nauseaTolerance;
        uint16_t timeInQueue;
        money64 cashInPocket;
        money64 cashSpent;
        RideId photo1RideRef;
        RideId photo2RideRef;
        RideId photo3RideRef;
        RideId photo4RideRef;

        int8_t rejoinQueueTimeout; // whilst waiting for a free vehicle (or pair) in the entrance
        RideId previousRide;
        uint16_t previousRideTimeOut;
        std::array<PeepThought, kPeepMaxThoughts> thoughts;
        // 0x3F Litter Count split into lots of 3 with time, 0xC0 Time since last recalc
        uint8_t litterCount;
        // 0x3F Sick Count split into lots of 3 with time, 0xC0 Time since last recalc
        uint8_t disgustingCount;
        uint8_t amountOfFood;
        uint8_t amountOfDrinks;
        uint8_t amountOfSouvenirs;
        uint8_t vandalismSeen; // 0xC0 vandalism thought timeout, 0x3F vandalism tiles seen
        uint8_t voucherType;
        union
        {
            RideId voucherRideId;
            ShopItemIndex voucherShopItem;
        };
        uint8_t surroundingsThoughtTimeout;
        uint8_t angriness;
        uint8_t timeLost; // the time the peep has been lost when it reaches 254 generates the lost thought
        uint8_t daysInQueue;
        Drawing::Colour balloonColour;
        Drawing::Colour umbrellaColour;
        Drawing::Colour hatColour;
        RideId favouriteRide;
        uint8_t favouriteRideRating;
        uint64_t itemFlags;

        void update();
        void tick128UpdateGuest(uint32_t index);
        uint64_t getFoodOrDrinkFlags() const;
        uint64_t getEmptyContainerFlags() const;
        bool hasDrink() const;
        bool hasFoodOrDrink() const;
        bool hasEmptyContainer() const;
        void updateAnimationGroup();
        bool headingForRideOrParkExit() const;
        void readMap();
        bool shouldGoOnRide(Ride& ride, StationIndex entranceNum, bool atQueue, bool thinking);
        void spendMoney(money64& peep_expend_type, money64 amount, ExpenditureType type);
        void spendMoney(money64 amount, ExpenditureType type);
        void setHasRidden(const Ride& ride);
        bool hasRidden(const Ride& ride) const;
        void setHasRiddenRideType(ride_type_t rideType);
        bool hasRiddenRideType(ride_type_t rideType) const;
        void setParkEntryTime(int32_t entryTime);
        int32_t getParkEntryTime() const;
        void checkIfLost();
        void checkCantFindRide();
        void checkCantFindExit();
        void setAnimationGroup(PeepAnimationGroup new_sprite_type);
        int32_t getEasterEggNameId() const;
        void updateEasterEggInteractions();
        void insertNewThought(PeepThoughtType thought_type);
        void insertNewThought(PeepThoughtType thought_type, ShopItem thought_arguments);
        void insertNewThought(PeepThoughtType thought_type, RideId rideId);
        void insertNewThought(PeepThoughtType thought_type, uint16_t thought_arguments);
        static Guest* generate(const CoordsXYZ& coords);
        bool updateQueuePosition(PeepActionType previous_action);
        void removeFromQueue();

        uint64_t getItemFlags() const;
        void setItemFlags(uint64_t value);
        void removeAllItems();
        void removeItem(ShopItem item);
        void giveItem(ShopItem item);
        bool hasItem(ShopItem peepItem) const;
        void serialise(DataSerialiser& stream);

        // Removes the ride from the guests memory, this includes
        // the history, thoughts, etc.
        void removeRideFromMemory(RideId rideId);
        void handleEasterEggName();
        void choseNotToGoOnRide(const Ride& ride, bool peepAtRide, bool updateLastRide);
        void onEnterRide(Ride& ride);
        void onExitRide(Ride& ride);
        void throwUp();

    private:
        bool shouldFindBench();
        bool updateWalkingFindBin();
        bool updateWalkingFindBench();
        void updateRide();
        void updateOnRide() {}; // TODO
        void updateWalking();
        void updateQueuing();
        void updateSitting();
        void updateEnteringPark();
        void updateLeavingPark();
        void updateBuying();
        void updateWatching();
        void updateUsingBin();
        void updateRideAtEntrance();
        void updateRideAdvanceThroughEntrance();
        void updateRideLeaveEntranceWaypoints(const Ride& ride);
        uint8_t getWaypointedSeatLocation(const Ride& ride, const CarEntry* vehicle_type, uint8_t track_direction) const;
        void updateRideFreeVehicleCheck();
        void updateRideFreeVehicleEnterRide(Ride& ride);
        void updateRideApproachVehicle();
        void updateRideEnterVehicle();
        void updateRideLeaveVehicle();
        void updateRideApproachExit();
        void updateRideInExit();

        void updateRideApproachVehicleWaypoints();

        void updateRideApproachExitWaypoints();
        void updateRideApproachSpiralSlide();
        void updateRideOnSpiralSlide();
        void updateRideLeaveSpiralSlide();
        void updateRideMazePathfinding();
        void updateRideLeaveExit();
        void updateRideShopApproach();
        void updateRideShopInteract();
        void updateRideShopLeave();
        void updateRidePrepareForExit();
        void updateMotivesIdle();
        void updateConsumptionMotives();
        int32_t checkEasterEggName(int32_t index) const;
        void givePassingGuestPurpleClothes(Guest& passingPeep);
        void givePassingGuestPizza(Guest& passingPeep);
        void makePassingGuestSick(Guest& passingPeep);
        void givePassingPeepsIceCream(Guest& passingPeep);
        void goToRideEntrance(const Ride& ride);
    };

    void updateRideApproachVehicleWaypointsMotionSimulator(Guest&, const CoordsXY&, int16_t&);
    void updateRideApproachVehicleWaypointsDefault(Guest&, const CoordsXY&, int16_t&);

    static_assert(sizeof(Guest) <= 512);

    enum
    {
        EASTEREGG_PEEP_NAME_MICHAEL_SCHUMACHER,
        EASTEREGG_PEEP_NAME_JACQUES_VILLENEUVE,
        EASTEREGG_PEEP_NAME_DAMON_HILL,
        EASTEREGG_PEEP_NAME_MR_BEAN,
        EASTEREGG_PEEP_NAME_CHRIS_SAWYER,
        EASTEREGG_PEEP_NAME_KATIE_BRAYSHAW,
        EASTEREGG_PEEP_NAME_MELANIE_WARN,
        EASTEREGG_PEEP_NAME_SIMON_FOSTER,
        EASTEREGG_PEEP_NAME_JOHN_WARDLEY,
        EASTEREGG_PEEP_NAME_LISA_STIRLING,
        EASTEREGG_PEEP_NAME_DONALD_MACRAE,
        EASTEREGG_PEEP_NAME_KATHERINE_MCGOWAN,
        EASTEREGG_PEEP_NAME_FRANCES_MCGOWAN,
        EASTEREGG_PEEP_NAME_CORINA_MASSOURA,
        EASTEREGG_PEEP_NAME_CAROL_YOUNG,
        EASTEREGG_PEEP_NAME_MIA_SHERIDAN,
        EASTEREGG_PEEP_NAME_KATIE_RODGER,
        EASTEREGG_PEEP_NAME_EMMA_GARRELL,
        EASTEREGG_PEEP_NAME_JOANNE_BARTON,
        EASTEREGG_PEEP_NAME_FELICITY_ANDERSON,
        EASTEREGG_PEEP_NAME_KATIE_SMITH,
        EASTEREGG_PEEP_NAME_EILIDH_BELL,
        EASTEREGG_PEEP_NAME_NANCY_STILLWAGON,
        EASTEREGG_PEEP_NAME_DAVID_ELLIS
    };

    void PeepThoughtSetFormatArgs(const PeepThought* thought, Formatter& ft);

    void IncrementGuestsInPark();
    void IncrementGuestsHeadingForPark();
    void DecrementGuestsInPark();
    void DecrementGuestsHeadingForPark();

    void PeepUpdateRideLeaveEntranceMaze(Guest& peep, Ride& ride, CoordsXYZD& entrance_loc);
    void PeepUpdateRideLeaveEntranceSpiralSlide(Guest& peep, Ride& ride, CoordsXYZD& entrance_loc);
    void PeepUpdateRideLeaveEntranceDefault(Guest& peep, Ride& ride, CoordsXYZD& entrance_loc);

    CoordsXY GetGuestWaypointLocationDefault(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation);
    CoordsXY GetGuestWaypointLocationEnterprise(
        const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation);
} // namespace OpenRCT2
