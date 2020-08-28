/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _PEEP_H_
#define _PEEP_H_

#include "../common.h"
#include "../management/Finance.h"
#include "../rct12/RCT12.h"
#include "../ride/Ride.h"
#include "../ride/RideTypes.h"
#include "../ride/ShopItem.h"
#include "../util/Util.h"
#include "../world/Location.hpp"
#include "../world/SpriteBase.h"

#include <algorithm>
#include <bitset>
#include <optional>

#define PEEP_MAX_THOUGHTS 5
#define PEEP_THOUGHT_ITEM_NONE 255

#define PEEP_HUNGER_WARNING_THRESHOLD 25
#define PEEP_THIRST_WARNING_THRESHOLD 25
#define PEEP_TOILET_WARNING_THRESHOLD 28
#define PEEP_LITTER_WARNING_THRESHOLD 23
#define PEEP_DISGUST_WARNING_THRESHOLD 22
#define PEEP_VANDALISM_WARNING_THRESHOLD 15
#define PEEP_NOEXIT_WARNING_THRESHOLD 8
#define PEEP_LOST_WARNING_THRESHOLD 8

#define PEEP_MAX_HAPPINESS 255
#define PEEP_MIN_ENERGY 32
#define PEEP_MAX_ENERGY 128
#define PEEP_MAX_ENERGY_TARGET 255 // Oddly, this differs from max energy!
#define PEEP_MAX_HUNGER 255
#define PEEP_MAX_TOILET 255
#define PEEP_MAX_NAUSEA 255
#define PEEP_MAX_THIRST 255

constexpr auto PEEP_CLEARANCE_HEIGHT = 4 * COORDS_Z_STEP;

class Formatter;
struct TileElement;
struct Ride;
namespace GameActions
{
    class Result;
}
using ParkEntranceIndex = uint8_t;

enum class StaffType : uint8_t
{
    Handyman,
    Mechanic,
    Security,
    Entertainer,

    Count
};

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

    None1 = 254,
    None2 = 255,
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

enum class PeepNauseaTolerance : uint8_t
{
    None,
    Low,
    Average,
    High
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

// Flags used by peep_should_go_on_ride()
enum PeepRideDecision
{
    PEEP_RIDE_DECISION_AT_QUEUE = 1,
    PEEP_RIDE_DECISION_THINKING = 1 << 2,
};

struct rct_peep_thought
{
    PeepThoughtType type;  // 0
    uint8_t item;          // 1
    uint8_t freshness;     // 2 larger is less fresh
    uint8_t fresh_timeout; // 3 updates every tick
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

struct Peep : SpriteBase
{
    char* Name;
    CoordsXYZ NextLoc;
    uint8_t NextFlags;
    bool OutsideOfPark;
    PeepState State;
    union
    {
        uint8_t SubState;
        PeepSittingSubState SittingSubState;
        PeepRideSubState RideSubState;
        PeepUsingBinSubState UsingBinSubState;
    };
    PeepSpriteType SpriteType;
    union
    {
        StaffType AssignedStaffType;
        uint8_t GuestNumRides;
    };
    uint8_t TshirtColour;
    uint8_t TrousersColour;
    uint16_t DestinationX; // Location that the peep is trying to get to
    uint16_t DestinationY;
    uint8_t DestinationTolerance; // How close to destination before next action/state 0 = exact
    uint8_t Var37;
    uint8_t Energy;
    uint8_t EnergyTarget;
    uint8_t Happiness;
    uint8_t HappinessTarget;
    uint8_t Nausea;
    uint8_t NauseaTarget;
    uint8_t Hunger;
    uint8_t Thirst;
    uint8_t Toilet;
    uint8_t Mass;
    uint8_t TimeToConsume;
    IntensityRange Intensity{ 0 };
    PeepNauseaTolerance NauseaTolerance;
    uint8_t WindowInvalidateFlags;
    money16 PaidOnDrink;
    uint8_t RideTypesBeenOn[16];
    ride_id_t Photo2RideRef;
    ride_id_t Photo3RideRef;
    ride_id_t Photo4RideRef;
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
        uint16_t MechanicTimeSinceCall; // time getting to ride to fix
        uint16_t GuestNextInQueue;
    };
    union
    {
        uint8_t MazeLastEdge;
        Direction PeepDirection; // Direction ?
    };
    ride_id_t InteractionRideIndex;
    uint16_t TimeInQueue;
    ride_id_t RidesBeenOn[32];
    // 255 bit bitmap of every ride the peep has been on see
    // window_peep_rides_update for how to use.
    uint32_t Id;
    money32 CashInPocket;
    money32 CashSpent;
    union
    {
        int32_t ParkEntryTime;
        int32_t HireDate;
    };
    int8_t RejoinQueueTimeout; // whilst waiting for a free vehicle (or pair) in the entrance
    ride_id_t PreviousRide;
    uint16_t PreviousRideTimeOut;
    rct_peep_thought Thoughts[PEEP_MAX_THOUGHTS];
    uint8_t PathCheckOptimisation; // see peep.checkForPath
    union
    {
        uint8_t StaffId;
        ride_id_t GuestHeadingToRideId;
    };
    union
    {
        uint8_t StaffOrders;
        uint8_t GuestIsLostCountdown;
    };
    ride_id_t Photo1RideRef;
    uint32_t PeepFlags;
    rct12_xyzd8 PathfindGoal;
    rct12_xyzd8 PathfindHistory[4];
    uint8_t WalkingFrameNum;
    // 0x3F Litter Count split into lots of 3 with time, 0xC0 Time since last recalc
    uint8_t LitterCount;
    union
    {
        uint8_t GuestTimeOnRide;
        uint8_t StaffMowingTimeout;
    };
    // 0x3F Sick Count split into lots of 3 with time, 0xC0 Time since last recalc
    uint8_t DisgustingCount;
    union
    {
        money16 PaidToEnter;
        uint16_t StaffLawnsMown;
        uint16_t StaffRidesFixed;
    };
    union
    {
        money16 PaidOnRides;
        uint16_t StaffGardensWatered;
        uint16_t StaffRidesInspected;
    };
    union
    {
        money16 PaidOnFood;
        uint16_t StaffLitterSwept;
        uint16_t StaffVandalsStopped;
    };
    union
    {
        money16 PaidOnSouvenirs;
        uint16_t StaffBinsEmptied;
    };
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

public: // Peep
    void Update();
    std::optional<CoordsXY> UpdateAction(int16_t& xy_distance);
    std::optional<CoordsXY> UpdateAction();
    void SetState(PeepState new_state);
    void Remove();
    void UpdateCurrentActionSpriteType();
    void SwitchToSpecialSprite(uint8_t special_sprite_id);
    void StateReset();
    uint8_t GetNextDirection() const;
    bool GetNextIsSloped() const;
    bool GetNextIsSurface() const;
    void SetNextFlags(uint8_t next_direction, bool is_sloped, bool is_surface);
    bool CanBePickedUp() const;
    void Pickup();
    void PickupAbort(int32_t old_x);
    std::unique_ptr<GameActions::Result> Place(const TileCoordsXYZ& location, bool apply);
    static Peep* Generate(const CoordsXYZ& coords);
    void RemoveFromQueue();
    void RemoveFromRide();
    void InsertNewThought(PeepThoughtType thought_type, uint8_t thought_arguments);
    void FormatActionTo(Formatter&) const;
    void FormatNameTo(Formatter&) const;
    std::string GetName() const;
    bool SetName(std::string_view value);

    // Reset the peep's stored goal, which means they will forget any stored pathfinding history
    // on the next peep_pathfind_choose_direction call.
    void ResetPathfindGoal();
    uint64_t GetItemFlags() const;
    void SetItemFlags(uint64_t itemFlags);
    void RemoveAllItems();
    void RemoveItem(ShopItem item);
    void GiveItem(ShopItem item);
    bool HasItem(ShopItem peepItem) const;

    void SetDestination(const CoordsXY& coords);
    void SetDestination(const CoordsXY& coords, int32_t tolerance);
    CoordsXY GetDestination() const;

    // TODO: Make these private again when done refactoring
public: // Peep
    bool CheckForPath();
    void PerformNextAction(uint8_t& pathing_result);
    void PerformNextAction(uint8_t& pathing_result, TileElement*& tile_result);
    int32_t GetZOnSlope(int32_t tile_x, int32_t tile_y);
    void SwitchNextActionSpriteType();
    PeepActionSpriteType GetActionSpriteType();

private:
    void UpdateFalling();
    void Update1();
    void UpdatePicked();
};

struct Guest : Peep
{
    static constexpr auto cEntityType = EntityType::Guest;

public:
    void UpdateGuest();
    void Tick128UpdateGuest(int32_t index);
    int64_t GetFoodOrDrinkFlags() const;
    int64_t GetEmptyContainerFlags() const;
    bool HasDrink() const;
    bool HasFoodOrDrink() const;
    bool HasEmptyContainer() const;
    void OnEnterRide(ride_id_t rideIndex);
    void OnExitRide(ride_id_t rideIndex);
    void UpdateSpriteType();
    bool HeadingForRideOrParkExit() const;
    void StopPurchaseThought(uint8_t ride_type);
    void TryGetUpFromSitting();
    void ChoseNotToGoOnRide(Ride* ride, bool peepAtRide, bool updateLastRide);
    void PickRideToGoOn();
    void ReadMap();
    bool ShouldGoOnRide(Ride* ride, int32_t entranceNum, bool atQueue, bool thinking);
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

private:
    void UpdateRide();
    void UpdateOnRide(){}; // TODO
    void UpdateWalking();
    void UpdateQueuing();
    void UpdateSitting();
    void UpdateEnteringPark();
    void UpdateLeavingPark();
    void UpdateBuying();
    void UpdateWatching();
    void UpdateUsingBin();
    void UpdateRideAtEntrance();
    void UpdateRideAdvanceThroughEntrance();
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
    void loc_68F9F3();
    void loc_68FA89();
    using easter_egg_function = void (Guest::*)(Guest* otherGuest);
    int32_t CheckEasterEggName(int32_t index) const;
    void ApplyEasterEggToNearbyGuests(easter_egg_function easter_egg);
    bool GuestHasValidXY() const;
    void GivePassingPeepsPurpleClothes(Guest* passingPeep);
    void GivePassingPeepsPizza(Guest* passingPeep);
    void MakePassingPeepsSick(Guest* passingPeep);
    void GivePassingPeepsIceCream(Guest* passingPeep);
    Ride* FindBestRideToGoOn();
    std::bitset<MAX_RIDES> FindRidesToGoOn();
    bool FindVehicleToEnter(Ride* ride, std::vector<uint8_t>& car_array);
    void GoToRideEntrance(Ride* ride);
};

struct Staff : Peep
{
    static constexpr auto cEntityType = EntityType::Staff;

public:
    void UpdateStaff(uint32_t stepsToTake);
    void Tick128UpdateStaff();
    bool IsMechanic() const;
    bool IsPatrolAreaSet(const CoordsXY& coords) const;
    bool IsLocationInPatrol(const CoordsXY& loc) const;
    bool IsLocationOnPatrolEdge(const CoordsXY& loc) const;
    bool DoPathFinding();
    uint8_t GetCostume() const;
    void SetCostume(uint8_t value);
    void SetHireDate(int32_t hireDate);
    int32_t GetHireDate() const;

    bool CanIgnoreWideFlag(const CoordsXYZ& staffPos, TileElement* path) const;

    static void ResetStats();

private:
    void UpdatePatrolling();
    void UpdateMowing();
    void UpdateSweeping();
    void UpdateEmptyingBin();
    void UpdateWatering();
    void UpdateAnswering();
    void UpdateFixing(int32_t steps);
    bool UpdateFixingEnterStation(Ride* ride) const;
    bool UpdateFixingMoveToBrokenDownVehicle(bool firstRun, const Ride* ride);
    bool UpdateFixingFixVehicle(bool firstRun, const Ride* ride);
    bool UpdateFixingFixVehicleMalfunction(bool firstRun, const Ride* ride);
    bool UpdateFixingMoveToStationEnd(bool firstRun, const Ride* ride);
    bool UpdateFixingFixStationEnd(bool firstRun);
    bool UpdateFixingMoveToStationStart(bool firstRun, const Ride* ride);
    bool UpdateFixingFixStationStart(bool firstRun, const Ride* ride);
    bool UpdateFixingFixStationBrakes(bool firstRun, Ride* ride);
    bool UpdateFixingMoveToStationExit(bool firstRun, const Ride* ride);
    bool UpdateFixingFinishFixOrInspect(bool firstRun, int32_t steps, Ride* ride);
    bool UpdateFixingLeaveByEntranceExit(bool firstRun, const Ride* ride);
    void UpdateRideInspected(ride_id_t rideIndex);
    void UpdateHeadingToInspect();

    bool DoHandymanPathFinding();
    bool DoMechanicPathFinding();
    bool DoEntertainerPathFinding();
    bool DoMiscPathFinding();

    Direction HandymanDirectionRandSurface(uint8_t validDirections) const;

    void EntertainerUpdateNearbyPeeps() const;

    uint8_t GetValidPatrolDirections(const CoordsXY& loc) const;
    Direction HandymanDirectionToNearestLitter() const;
    uint8_t HandymanDirectionToUncutGrass(uint8_t valid_directions) const;
    Direction DirectionSurface(Direction initialDirection) const;
    Direction DirectionPath(uint8_t validDirections, PathElement* pathElement) const;
    Direction MechanicDirectionSurface() const;
    Direction MechanicDirectionPathRand(uint8_t pathDirections) const;
    Direction MechanicDirectionPath(uint8_t validDirections, PathElement* pathElement);
    bool UpdatePatrollingFindWatering();
    bool UpdatePatrollingFindBin();
    bool UpdatePatrollingFindSweeping();
    bool UpdatePatrollingFindGrass();
};

static_assert(sizeof(Peep) <= 512);

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

extern uint8_t gGuestChangeModifier;
extern uint32_t gNumGuestsInPark;
extern uint32_t gNumGuestsInParkLastWeek;
extern uint32_t gNumGuestsHeadingForPark;

extern money16 gGuestInitialCash;
extern uint8_t gGuestInitialHappiness;
extern uint8_t gGuestInitialHunger;
extern uint8_t gGuestInitialThirst;

extern uint32_t gNextGuestNumber;

extern uint8_t gPeepWarningThrottle[16];

Peep* try_get_guest(uint16_t spriteIndex);
int32_t peep_get_staff_count();
void peep_update_all();
void peep_problem_warnings_update();
void peep_stop_crowd_noise();
void peep_update_crowd_noise();
void peep_update_days_in_queue();
void peep_applause();
void peep_thought_set_format_args(const rct_peep_thought* thought, Formatter& ft);
int32_t get_peep_face_sprite_small(Peep* peep);
int32_t get_peep_face_sprite_large(Peep* peep);
void peep_sprite_remove(Peep* peep);

void peep_window_state_update(Peep* peep);
void peep_decrement_num_riders(Peep* peep);

void peep_set_map_tooltip(Peep* peep);
int32_t peep_compare(const uint16_t sprite_index_a, const uint16_t sprite_index_b);

void peep_update_names(bool realNames);

void guest_set_name(uint16_t spriteIndex, const char* name);

void increment_guests_in_park();
void increment_guests_heading_for_park();
void decrement_guests_in_park();
void decrement_guests_heading_for_park();

rct_string_id get_real_name_string_id_from_id(uint32_t id);

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

#endif
