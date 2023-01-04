/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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

#define PEEP_MAX_THOUGHTS 5

#define PEEP_HUNGER_WARNING_THRESHOLD 25
#define PEEP_THIRST_WARNING_THRESHOLD 25
#define PEEP_TOILET_WARNING_THRESHOLD 28
#define PEEP_LITTER_WARNING_THRESHOLD 23
#define PEEP_DISGUST_WARNING_THRESHOLD 22
#define PEEP_VANDALISM_WARNING_THRESHOLD 15
#define PEEP_NOEXIT_WARNING_THRESHOLD 8
#define PEEP_LOST_WARNING_THRESHOLD 8
#define PEEP_TOO_LONG_QUEUE_THRESHOLD 25

#define PEEP_MAX_HAPPINESS 255
#define PEEP_MAX_HUNGER 255
#define PEEP_MAX_TOILET 255
#define PEEP_MAX_NAUSEA 255
#define PEEP_MAX_THIRST 255

enum class PeepThoughtType : uint8_t
{
    CantAffordRide = 0,   // "I can't afford"
    SpentMoney = 1,       // "I've spent all my money"
    Sick = 2,             // "I feel sick"
    VerySick = 3,         // "I feel very sick"
    MoreThrilling = 4,    // "I want to go on something more thrilling than X"
    Intense = 5,          // "X looks too intense for me"
    HaventFinished = 6,   // "I haven't finished my X yet"
    Sickening = 7,        // "Just looking at X makes me feel sick"
    BadValue = 8,         // "I'm not paying that much to go on X"
    GoHome = 9,           // "I want to go home"
    GoodValue = 10,       // "X is really good value"
    AlreadyGot = 11,      // "I've already got"
    CantAffordItem = 12,  // "I can't afford"
    NotHungry = 13,       // "I'm not hungry"
    NotThirsty = 14,      // "I'm not thirsty"
    Drowning = 15,        // "Help! I'm drowning!"
    Lost = 16,            // "I'm lost!"
    WasGreat = 17,        // "X was great"
    QueuingAges = 18,     // "I've been queuing for X for ages"
    Tired = 19,           // "I'm tired"
    Hungry = 20,          // "I'm hungry"
    Thirsty = 21,         // "I'm thirsty"
    Toilet = 22,          // "I need to go to the toilet"
    CantFind = 23,        // "I can't find X"
    NotPaying = 24,       // "I'm not paying that much to use X"
    NotWhileRaining = 25, // "I'm not going on X while it's raining"
    BadLitter = 26,       // "The litter here is really bad"
    CantFindExit = 27,    // "I can't find the exit"
    GetOff = 28,          // "I want to get off X"
    GetOut = 29,          // "I want to get out of X"
    NotSafe = 30,         // "I'm not going on X - it isn't safe"
    PathDisgusting = 31,  // "This path is disgusting"
    Crowded = 32,         // "It's too crowded here"
    Vandalism = 33,       // "The vandalism here is really bad"
    Scenery = 34,         // "Great scenery!"
    VeryClean = 35,       // "This park is very clean and tidy"
    Fountains = 36,       // "The jumping fountains are great"
    Music = 37,           // "The music is nice here"
    Balloon = 38,         // "This balloon from X is really good value"
    Toy = 39,             // "This cuddly toy from X is really good value"
    Map = 40,
    Photo = 41, // "This on ride photo from X is really good value"
    Umbrella = 42,
    Drink = 43,
    Burger = 44,
    Chips = 45,
    IceCream = 46,
    Candyfloss = 47,

    Pizza = 51,

    Popcorn = 53,
    HotDog = 54,
    Tentacle = 55,
    Hat = 56,
    ToffeeApple = 57,
    Tshirt = 58,
    Doughnut = 59,
    Coffee = 60,

    Chicken = 62,
    Lemonade = 63,

    Wow = 67, // "Wow!"

    Wow2 = 70,        // "Wow!"
    Watched = 71,     // "I have the strangest feeling someone is watching me"
    BalloonMuch = 72, // "I'm not paying that much to get a balloon from X"
    ToyMuch = 73,
    MapMuch = 74,
    PhotoMuch = 75,
    UmbrellaMuch = 76,
    DrinkMuch = 77,
    BurgerMuch = 78,
    ChipsMuch = 79,
    IceCreamMuch = 80,
    CandyflossMuch = 81,

    PizzaMuch = 85,

    PopcornMuch = 87,
    HotDogMuch = 88,
    TentacleMuch = 89,
    HatMuch = 90,
    ToffeeAppleMuch = 91,
    TshirtMuch = 92,
    DoughnutMuch = 93,
    CoffeeMuch = 94,

    ChickenMuch = 96,
    LemonadeMuch = 97,

    Photo2 = 104, // "This on-ride photo from X is really good value"
    Photo3 = 105,
    Photo4 = 106,
    Pretzel = 107,
    HotChocolate = 108,
    IcedTea = 109,
    FunnelCake = 110,
    Sunglasses = 111,
    BeefNoodles = 112,
    FriedRiceNoodles = 113,
    WontonSoup = 114,
    MeatballSoup = 115,
    FruitJuice = 116,
    SoybeanMilk = 117,
    Sujongkwa = 118,
    SubSandwich = 119,
    Cookie = 120,

    RoastSausage = 124,

    Photo2Much = 136,
    Photo3Much = 137,
    Photo4Much = 138,
    PretzelMuch = 139,
    HotChocolateMuch = 140,
    IcedTeaMuch = 141,
    FunnelCakeMuch = 142,
    SunglassesMuch = 143,
    BeefNoodlesMuch = 144,
    FriedRiceNoodlesMuch = 145,
    WontonSoupMuch = 146,
    MeatballSoupMuch = 147,
    FruitJuiceMuch = 148,
    SoybeanMilkMuch = 149,
    SujongkwaMuch = 150,
    SubSandwichMuch = 151,
    CookieMuch = 152,

    RoastSausageMuch = 156,

    Help = 168,               // "Help! Put me down!"
    RunningOut = 169,         // "I'm running out of cash!"
    NewRide = 170,            // "Wow! A new ride being built!"
    NiceRideDeprecated = 171, // "Nice ride! But not as good as the Phoenix..."
    ExcitedDeprecated = 172,  // "I'm so excited - It's an Intamin ride!"
    HereWeAre = 173,          // "...and here we are on X!"

    None = 255
};

enum class PeepNauseaTolerance : uint8_t
{
    None,
    Low,
    Average,
    High
};

static constexpr uint16_t PeepThoughtItemNone = std::numeric_limits<uint16_t>::max();

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
struct CarEntry;

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
    static constexpr auto cEntityType = EntityType::Guest;

public:
    uint8_t GuestNumRides;
    EntityId GuestNextInQueue;
    int32_t ParkEntryTime;
    RideId GuestHeadingToRideId;
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
    uint16_t TimeInQueue;
    money32 CashInPocket;
    money32 CashSpent;
    RideId Photo1RideRef;
    RideId Photo2RideRef;
    RideId Photo3RideRef;
    RideId Photo4RideRef;

    int8_t RejoinQueueTimeout; // whilst waiting for a free vehicle (or pair) in the entrance
    RideId PreviousRide;
    uint16_t PreviousRideTimeOut;
    std::array<PeepThought, PEEP_MAX_THOUGHTS> Thoughts;
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
        RideId VoucherRideId;
        ShopItemIndex VoucherShopItem;
    };
    uint8_t SurroundingsThoughtTimeout;
    uint8_t Angriness;
    uint8_t TimeLost; // the time the peep has been lost when it reaches 254 generates the lost thought
    uint8_t DaysInQueue;
    uint8_t BalloonColour;
    uint8_t UmbrellaColour;
    uint8_t HatColour;
    RideId FavouriteRide;
    uint8_t FavouriteRideRating;
    uint64_t ItemFlags;

    void UpdateGuest();
    void Tick128UpdateGuest(int32_t index);
    int64_t GetFoodOrDrinkFlags() const;
    int64_t GetEmptyContainerFlags() const;
    bool HasDrink() const;
    bool HasFoodOrDrink() const;
    bool HasEmptyContainer() const;
    void OnEnterRide(Ride* ride);
    void OnExitRide(Ride* ride);
    void UpdateSpriteType();
    bool HeadingForRideOrParkExit() const;
    void StopPurchaseThought(ride_type_t rideType);
    void TryGetUpFromSitting();
    bool ShouldRideWhileRaining(const Ride& ride);
    void ChoseNotToGoOnRide(Ride* ride, bool peepAtRide, bool updateLastRide);
    void PickRideToGoOn();
    void ReadMap();
    bool ShouldGoOnRide(Ride* ride, StationIndex entranceNum, bool atQueue, bool thinking);
    bool ShouldGoToShop(Ride* ride, bool peepAtShop);
    bool ShouldFindBench();
    bool UpdateWalkingFindBench();
    bool UpdateWalkingFindBin();
    void SpendMoney(money16& peep_expend_type, money32 amount, ExpenditureType type);
    void SpendMoney(money32 amount, ExpenditureType type);
    void SetHasRidden(const Ride* ride);
    bool HasRidden(const Ride* ride) const;
    void SetHasRiddenRideType(int32_t rideType);
    bool HasRiddenRideType(int32_t rideType) const;
    void SetParkEntryTime(int32_t entryTime);
    int32_t GetParkEntryTime() const;
    void CheckIfLost();
    void CheckCantFindRide();
    void CheckCantFindExit();
    bool DecideAndBuyItem(Ride* ride, ShopItem shopItem, money32 price);
    void SetSpriteType(PeepSpriteType new_sprite_type);
    void HandleEasterEggName();
    int32_t GetEasterEggNameId() const;
    void UpdateEasterEggInteractions();
    void InsertNewThought(PeepThoughtType thought_type);
    void InsertNewThought(PeepThoughtType thought_type, ShopItem thought_arguments);
    void InsertNewThought(PeepThoughtType thought_type, RideId rideId);
    void InsertNewThought(PeepThoughtType thought_type, uint16_t thought_arguments);
    static Guest* Generate(const CoordsXYZ& coords);
    bool UpdateQueuePosition(PeepActionType previous_action);
    void RemoveFromQueue();

    uint64_t GetItemFlags() const;
    void SetItemFlags(uint64_t itemFlags);
    void RemoveAllItems();
    void RemoveItem(ShopItem item);
    void GiveItem(ShopItem item);
    bool HasItem(ShopItem peepItem) const;
    void Serialise(DataSerialiser& stream);

    // Removes the ride from the guests memory, this includes
    // the history, thoughts, etc.
    void RemoveRideFromMemory(RideId rideId);

private:
    void UpdateRide();
    void UpdateOnRide(){}; // TODO
    void UpdateWalking();
    void UpdateWaitingAtCrossing();
    void UpdateQueuing();
    void UpdateSitting();
    void UpdateEnteringPark();
    void UpdateLeavingPark();
    void UpdateBuying();
    void UpdateWatching();
    void UpdateUsingBin();
    void UpdateRideAtEntrance();
    void UpdateRideAdvanceThroughEntrance();
    void UpdateRideLeaveEntranceWaypoints(const Ride& ride);
    uint8_t GetWaypointedSeatLocation(const Ride& ride, CarEntry* vehicle_type, uint8_t track_direction) const;
    void UpdateRideFreeVehicleCheck();
    void UpdateRideFreeVehicleEnterRide(Ride* ride);
    void UpdateRideApproachVehicle();
    void UpdateRideEnterVehicle();
    void UpdateRideLeaveVehicle();
    void UpdateRideApproachExit();
    void UpdateRideInExit();

    void UpdateRideApproachVehicleWaypoints();

    void UpdateRideApproachExitWaypoints();
    void UpdateRideApproachSpiralSlide();
    void UpdateRideOnSpiralSlide();
    void UpdateRideLeaveSpiralSlide();
    void UpdateRideMazePathfinding();
    void UpdateRideLeaveExit();
    void UpdateRideShopApproach();
    void UpdateRideShopInteract();
    void UpdateRideShopLeave();
    void UpdateRidePrepareForExit();
    void loc_68F9F3();
    void loc_68FA89();
    int32_t CheckEasterEggName(int32_t index) const;
    bool GuestHasValidXY() const;
    void GivePassingPeepsPurpleClothes(Guest* passingPeep);
    void GivePassingPeepsPizza(Guest* passingPeep);
    void MakePassingPeepsSick(Guest* passingPeep);
    void GivePassingPeepsIceCream(Guest* passingPeep);
    Ride* FindBestRideToGoOn();
    OpenRCT2::BitSet<OpenRCT2::Limits::MaxRidesInPark> FindRidesToGoOn();
    bool FindVehicleToEnter(Ride* ride, std::vector<uint8_t>& car_array);
    void GoToRideEntrance(Ride* ride);
};

void UpdateRideApproachVehicleWaypointsMotionSimulator(Guest&, const CoordsXY&, int16_t&);
void UpdateRideApproachVehicleWaypointsDefault(Guest&, const CoordsXY&, int16_t&);

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

extern uint8_t gGuestChangeModifier;
extern uint32_t gNumGuestsInPark;
extern uint32_t gNumGuestsInParkLastWeek;
extern uint32_t gNumGuestsHeadingForPark;

extern money16 gGuestInitialCash;
extern uint8_t gGuestInitialHappiness;
extern uint8_t gGuestInitialHunger;
extern uint8_t gGuestInitialThirst;

extern uint32_t gNextGuestNumber;

void peep_thought_set_format_args(const PeepThought* thought, Formatter& ft);

void increment_guests_in_park();
void increment_guests_heading_for_park();
void decrement_guests_in_park();
void decrement_guests_heading_for_park();

void PeepUpdateRideLeaveEntranceMaze(Guest* peep, Ride* ride, CoordsXYZD& entrance_loc);
void PeepUpdateRideLeaveEntranceSpiralSlide(Guest* peep, Ride* ride, CoordsXYZD& entrance_loc);
void PeepUpdateRideLeaveEntranceDefault(Guest* peep, Ride* ride, CoordsXYZD& entrance_loc);

CoordsXY GetGuestWaypointLocationDefault(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation);
CoordsXY GetGuestWaypointLocationEnterprise(const Vehicle& vehicle, const Ride& ride, const StationIndex& CurrentRideStation);
