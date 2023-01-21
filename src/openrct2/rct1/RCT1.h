/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../rct12/RCT12.h"
#include "../ride/RideRatings.h"
#include "Limits.h"

namespace RCT1
{
    constexpr const uint8_t RCT1ResearchFlagsSeparator = 0xFF;

    enum class RideType : uint8_t
    {
        WoodenRollerCoaster = 0,
        StandUpSteelRollerCoaster,
        SuspendedRollerCoaster,
        InvertedRollerCoaster,
        SteelMiniRollerCoaster,
        MiniatureRailway,
        Monorail,
        SuspendedSingleRailRollerCoaster,
        BoatHire,
        WoodenCrazyRodentRollerCoaster,
        SingleRailRollerCoaster,
        CarRide,
        LaunchedFreefall,
        BobsledRollerCoaster,
        ObservationTower,
        SteelRollerCoaster,
        WaterSlide,
        MineTrainRollerCoaster,
        Chairlift,
        SteelCorkscrewRollerCoaster,
        HedgeMaze,
        SpiralSlide,
        GoKarts,
        LogFlume,
        RiverRapids,
        Dodgems,
        SwingingShip,
        SwingingInverterShip,
        IceCreamStall,
        ChipsStall,
        DrinkStall,
        CandyflossStall,
        BurgerBar,
        MerryGoRound,
        BalloonStall,
        InformationKiosk,
        Toilets,
        FerrisWheel,
        MotionSimulator,
        _3DCinema,
        TopSpin,
        SpaceRings,
        ReverseFreefallRollerCoaster,
        SouvenirStall,
        VerticalRollerCoaster,
        PizzaStall,
        Twist,
        HauntedHouse,
        PopcornStall,
        Circus,
        GhostTrain,
        SteelTwisterRollerCoaster,
        WoodenTwisterRollerCoaster,
        WoodenSideFrictionRollerCoaster,
        SteelWildMouseRollerCoaster,
        HotDogStall,
        ExoticSeaFoodStall,
        HatStall,
        ToffeeAppleStall,
        VirginiaReel,
        RiverRide,
        CycleMonorail,
        FlyingRollerCoaster,
        SuspendedMonorail,
        _40,
        WoodenReverseRollerCoaster,
        HeartlineTwisterRollerCoaster,
        MiniatureGolf,
        _44,
        RotoDrop,
        FlyingSaucers,
        CrookedHouse,
        CycleRailway,
        SuspendedLoopingRollerCoaster,
        WaterCoaster,
        AirPoweredVerticalCoaster,
        InvertedWildMouseCoaster,
        JetSkis,
        TShirtStall,
        RaftRide,
        DoughnutShop,
        Enterprise,
        CoffeeShop,
        FriedChickenStall,
        LemonadeStall,

        Count,
        Null = 255,
    };

#pragma pack(push, 1)
    struct Entrance
    {
        uint16_t x;
        uint16_t y;
        uint16_t z;
        uint8_t direction;
    };
    assert_struct_size(Entrance, 7);

    /**
     * RCT1 ride structure
     * size: 0x260
     */
    struct Ride
    {
        RideType Type;           // 0x000
        uint8_t VehicleType;     // 0x001
        uint16_t LifecycleFlags; // 0x002
        uint8_t OperatingMode;   // 0x004
        uint8_t ColourScheme;    // 0x005
        struct
        {
            colour_t Body;
            colour_t Trim;
        } VehicleColours[Limits::MaxTrainsPerRide];           // 0x006
        colour_t TrackPrimaryColour;                          // 0x01E
        colour_t TrackSecondaryColour;                        // 0x01F
        colour_t TrackSupportColour;                          // 0x020
        uint8_t Status;                                       // 0x021
        uint16_t Name;                                        // 0x022
        uint16_t NameArgumentRide;                            // 0x024
        uint16_t NameArgumentNumber;                          // 0x026
        RCT12xy8 OverallView;                                 // 0x028
        RCT12xy8 StationStarts[Limits::MaxStationsPerRide];   // 0x02A
        uint8_t StationHeights[Limits::MaxStationsPerRide];   // 0x032
        uint8_t StationLengths[Limits::MaxStationsPerRide];   // 0x036
        uint8_t StationLights[Limits::MaxStationsPerRide];    // 0x03A
        uint8_t StationDeparts[Limits::MaxStationsPerRide];   // 0x03E
        RCT12xy8 Entrances[Limits::MaxStationsPerRide];       // 0x042
        RCT12xy8 Exits[Limits::MaxStationsPerRide];           // 0x04A
        uint16_t LastPeepInQueue[Limits::MaxStationsPerRide]; // 0x052
        uint8_t NumPeepsInQueue[Limits::MaxStationsPerRide];  // 0x05A
        uint16_t Vehicles[Limits::MaxTrainsPerRide];          // 0x05E
        uint8_t DepartFlags;                                  // 0x076
        uint8_t NumStations;                                  // 0x077
        uint8_t NumTrains;                                    // 0x078
        uint8_t NumCarsPerTrain;                              // 0x079
        uint8_t ProposedNumTrains;                            // 0x07A
        uint8_t ProposedNumCarsPerTrain;                      // 0x07B
        uint8_t MaxTrains;                                    // 0x07C
        uint8_t MinMaxCarsPerTrain;                           // 0x07D
        uint8_t MinWaitingTime;                               // 0x07E
        uint8_t MaxWaitingTime;                               // 0x07F
        uint8_t OperationOption;                              // 0x080
        uint8_t BoatHireReturnDirection;                      // 0x081
        RCT12xy8 BoatHireReturnPosition;                      // 0x082
        uint8_t DataLoggingIndex;                             // 0x084
        uint8_t SpecialTrackElements;                         // 0x085
        uint16_t Unk6;                                        // 0x086
        int32_t MaxSpeed;                                     // 0x088
        int32_t AverageSpeed;                                 // 0x08C
        uint8_t CurrentTestSegment;                           // 0x090
        uint8_t AverageSpeedTestTimeout;                      // 0x091
        uint8_t Pad0E2[0x2];                                  // 0x092
        int32_t Length[Limits::MaxStationsPerRide];           // 0x094
        uint16_t Time[Limits::MaxStationsPerRide];            // 0x0A4
        fixed16_2dp MaxPositiveVerticalG;                     // 0x0AC
        fixed16_2dp MaxNegativeVerticalG;                     // 0x0AE
        fixed16_2dp MaxLateralG;                              // 0x0B0
        fixed16_2dp PreviousVerticalG;                        // 0x0B2
        fixed16_2dp PreviousLateralG;                         // 0x0B4
        uint8_t PadB6[0x2];                                   // 0x0B6
        uint32_t TestingFlags;                                // 0x0B8
        // x y map location of the current track piece during a test
        // this is to prevent counting special tracks multiple times
        RCT12xy8 CurTestTrackLocation; // 0x0BC
        // Next 3 variables are related (XXXX XYYY ZZZa aaaa)
        uint16_t TurnCountDefault; // 0x0BE X = current turn count
        uint16_t TurnCountBanked;  // 0x0C0
        uint16_t TurnCountSloped;  // 0x0C2 X = number turns > 3 elements
        union
        {
            uint8_t NumInversions; // 0x0C4
            uint8_t NumHoles;
        };
        uint8_t NumDrops;             // 0x0C5
        uint8_t StartDropHeight;      // 0x0C6
        uint8_t HighestDropHeight;    // 0x0C7
        int32_t ShelteredLength;      // 0x0C8
        uint8_t UnkCC[2];             // 0x0CC
        uint8_t NumShelteredSections; // 0x0CE
        // see CurTestTrackLocation
        uint8_t CurTestTrackZ; // 0x0CF
        int16_t UnkD0;         // 0x0D0
        int16_t UnkD2;         // 0x0D2
        // Customer count in the last 10 * 960 game ticks (sliding window)
        uint16_t NumCustomers[Limits::CustomerHistorySize]; // 0xD4
        money16 Price;                                      // 0x0E8
        RCT12xy8 ChairliftBullwheelLocation[2];             // 0x0EA
        uint8_t ChairliftBullwheelZ[2];                     // 0x0EE
        union
        {
            RatingTuple Ratings;
            struct
            {
                ride_rating Excitement; // 0x0F0
                ride_rating Intensity;  // 0x0F2
                ride_rating Nausea;     // 0x0F4
            };
        };
        uint16_t Value;                      // 0x0F6
        uint16_t ChairliftBullwheelRotation; // 0x0F8
        uint8_t Satisfaction;                // 0x0FA
        uint8_t SatisfactionTimeOut;         // 0x0FB
        uint8_t SatisfactionNext;            // 0x0FC
        uint8_t WindowInvalidateFlags;       // 0x0FD
        uint8_t UnkFE[2];                    // 0x0FE
        uint32_t TotalCustomers;             // 0x100
        money32 TotalProfit;                 // 0x104
        uint8_t Popularity;                  // 0x108
        uint8_t PopularityTimeOut;           // 0x109
        uint8_t PopularityNext;              // 0x10A
        uint8_t NumRiders;                   // 0x10B
        uint8_t MusicTuneID;                 // 0x10C
        uint8_t SlideInUse;                  // 0x10D
        union
        {
            uint16_t SlidePeep; // 0x10E
            uint16_t MazeTiles; // 0x10E
        };
        uint8_t Pad110[0xE];            // 0x110
        uint8_t SlidePeepTshirtColour;  // 0x11E
        uint8_t Pad11F[0x7];            // 0x11F
        uint8_t SpiralSlideProgress;    // 0x126
        uint8_t Pad127[0x9];            // 0x127
        int16_t BuildDate;              // 0x130
        money16 UpkeepCost;             // 0x131
        uint16_t RaceWinner;            // 0x132
        uint8_t Unk134[2];              // 0x134
        uint32_t MusicPosition;         // 0x138
        uint8_t BreakdownReasonPending; // 0x13C
        uint8_t MechanicStatus;         // 0x13D
        uint16_t Mechanic;              // 0x13E
        uint8_t InspectionStation;      // 0x140
        uint8_t BrokenVehicle;          // 0x141
        uint8_t BrokenCar;              // 0x142
        uint8_t BreakdownReason;        // 0x143
        money16 PriceSecondary;         // 0x144
        union
        {
            struct
            {
                uint8_t ReliabilitySubvalue;   // 0x146, 0 - 255, acts like the decimals for ReliabilityPercentage
                uint8_t ReliabilityPercentage; // 0x147, Starts at 100 and decreases from there.
            };
            uint16_t Reliability; // 0x146
        };
        uint8_t UnreliabilityFactor;                   // 0x148
        uint8_t Downtime;                              // 0x149
        uint8_t InspectionInterval;                    // 0x14A
        uint8_t LastInspection;                        // 0x14B
        uint8_t Unk14C[20];                            // 0x14C
        money32 IncomePerHour;                         // 0x160
        money32 Profit;                                // 0x164
        uint8_t QueueTime[Limits::MaxStationsPerRide]; // 0x168
        colour_t TrackColourMain[4];                   // 0x16C
        colour_t TrackColourAdditional[4];             // 0x170
        colour_t TrackColourSupports[4];               // 0x174
        uint8_t Music;                                 // 0x178
        uint8_t EntranceStyle;                         // 0x179
        uint8_t Unk17A[230];                           // 0x17A
    };
    assert_struct_size(Ride, 0x260);

    struct UnkEntity : RCT12EntityBase
    {
        uint8_t Pad1F[3];      // 0x1f
        StringId NameStringID; // 0x22
        uint16_t Var24;
        uint16_t Frame; // 0x26
        uint8_t Var28[3];
        uint8_t Var2B;
        uint8_t Pad2C[0x45];
        uint8_t Var71;
    };

    struct Vehicle : RCT12EntityBase
    {
        uint8_t Pitch;        // 0x1F
        uint8_t BankRotation; // 0x20
        uint8_t Pad21[3];
        int32_t RemainingDistance;  // 0x24
        int32_t Velocity;           // 0x28
        int32_t Acceleration;       // 0x2C
        uint8_t Ride;               // 0x30
        uint8_t CarType;            // 0x31
        RCT12VehicleColour Colours; // 0x32
        union
        {
            uint16_t TrackProgress; // 0x34
            struct
            {
                int8_t Var34;
                uint8_t Var35;
            };
        };
        union
        {
            int16_t TrackTypeAndDirection; // 0x36
            RCT12xy8 BoatLocation;         // 0x36
        };
        uint16_t TrackX;             // 0x38
        uint16_t TrackY;             // 0x3A
        uint16_t TrackZ;             // 0x3C
        uint16_t NextVehicleOnTrain; // 0x3E

        // The previous vehicle on the same train or the last vehicle on the previous or only train.
        uint16_t PrevVehicleOnRide; // 0x40

        // The next vehicle on the same train or the first vehicle on the next or only train
        uint16_t NextVehicleOnRide; // 0x42

        uint16_t Var44;
        uint16_t Mass;        // 0x46
        uint16_t UpdateFlags; // 0x48
        uint8_t SwingSprite;
        uint8_t CurrentStation; // 0x4B
        union
        {
            int16_t SwingPosition; // 0x4C
            int16_t CurrentTime;   // 0x4C
            struct
            {
                int8_t FerrisWheelVar0; // 0x4C
                int8_t FerrisWheelVar1; // 0x4D
            };
        };
        int16_t SwingSpeed;
        uint8_t Status;                // 0x50
        uint8_t SubState;              // 0x51
        uint16_t Peep[32];             // 0x52
        uint8_t PeepTshirtColours[32]; // 0x92
        uint8_t NumSeats;              // 0xB2
        uint8_t NumPeeps;              // 0xB3
        uint8_t NextFreeSeat;          // 0xB4
        uint8_t RestraintsPosition;    // 0xB5 0 == Close, 255 == Open
        int16_t SpinSpeed;
        uint16_t Sound2Flags;
        uint8_t SpinSprite;
        uint8_t Sound1ID;     // 0xBB
        uint8_t Sound1Volume; // 0xBC
        uint8_t Sound2ID;     // 0xBD
        uint8_t Sound2Volume; // 0xBE
        int8_t SoundVectorFactor;
        union
        {
            uint16_t VarC0;
            uint16_t TimeWaiting;     // 0xC0
            uint16_t CableLiftTarget; // 0xC0
        };
        uint8_t Speed;               // 0xC2
        uint8_t PoweredAcceleration; // 0xC3
        union
        {
            uint8_t DodgemsCollisionDetection; // 0xC4
            uint8_t CollisionDetectionTimer;   // 0xC4
        };
        uint8_t AnimationFrame;
        uint8_t PadC6[0x2];
        uint32_t AnimationState;
        uint8_t ScreamSoundID; // 0xCC
        uint8_t TrackSubposition;
        uint8_t NumLaps;              // 0xCE
        uint8_t BrakeSpeed;           // 0xCF
        uint16_t LostTimeOut;         // 0xD0
        int8_t VerticalDropCountdown; // 0xD1
        uint8_t VarD3;
        uint8_t MiniGolfCurrentAnimation;
        uint8_t MiniGolfFlags;   // 0xD5
        uint8_t RideSubtype;     // 0xD6
        uint8_t ColoursExtended; // 0xD7

        uint16_t GetTrackType() const
        {
            return TrackTypeAndDirection >> 2;
        }

        uint8_t GetTrackDirection() const
        {
            return TrackTypeAndDirection & RCT12VehicleTrackDirectionMask;
        }
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

        Balloon = 16,
        Candyfloss = 17,
        Umbrella = 18,
        Pizza = 19,       // Unsure
        SecurityAlt = 20, // Unknown
        Popcorn = 21,
        ArmsCrossed = 22,
        HeadDown = 23,
        Nauseous = 24,
        VeryNauseous = 25,
        RequireToilet = 26,
        Hat = 27,
        Burger = 28,
        Tentacle = 29,
        ToffeeApple = 30
    };

    struct Peep : RCT12EntityBase
    {
        uint8_t Pad1F[3];
        StringId NameStringID;     // 0x22
        uint16_t NextX;            // 0x24
        uint16_t NextY;            // 0x26
        uint8_t NextZ;             // 0x28
        uint8_t NextFlags;         // 0x29
        uint8_t OutsideOfPark;     // 0x2A
        uint8_t State;             // 0x2B
        uint8_t SubState;          // 0x2C
        PeepSpriteType SpriteType; // 0x2D
        RCT12PeepType PeepType;    // 0x2E
        union
        {
            uint8_t StaffType; // 0x2F
            uint8_t NumRides;  // 0x2F
        };
        uint8_t TshirtColour;         // 0x30
        uint8_t TrousersColour;       // 0x31
        uint16_t DestinationX;        // 0x32 Location that the peep is trying to get to
        uint16_t DestinationY;        // 0x34
        uint8_t DestinationTolerance; // 0x36 How close to destination before next action/state 0 = exact
        uint8_t Var37;
        uint8_t Energy;          // 0x38
        uint8_t EnergyTarget;    // 0x39
        uint8_t Happiness;       // 0x3A
        uint8_t HappinessTarget; // 0x3B
        uint8_t Nausea;          // 0x3C
        uint8_t NauseaTarget;    // 0x3D
        uint8_t Hunger;          // 0x3E
        uint8_t Thirst;          // 0x3F
        uint8_t Toilet;          // 0x40
        uint8_t Mass;            // 0x41
        uint8_t TimeToConsume;
        uint8_t Intensity; // 0x43 The max intensity is stored in the first 4 bits, and the min intensity in the second 4 bits
        uint8_t NauseaTolerance;        // 0x44
        uint8_t WindowInvalidateFlags;  // 0x45
        money16 PaidOnDrink;            // 0x46
        uint8_t ride_types_been_on[16]; // 0x48
        uint8_t Pad5F[0x10];            // 0x58
        RCT12RideId CurrentRide;        // 0x68
        uint8_t CurrentRideStation;     // 0x69
        uint8_t CurrentTrain;           // 0x6A
        union
        {
            struct
            {
                uint8_t CurrentCar;  // 0x6B
                uint8_t CurrentSeat; // 0x6C
            };
            uint16_t TimeToSitDown; // 0x6B
            struct
            {
                uint8_t TimeToStand;   // 0x6B
                uint8_t StandingFlags; // 0x6C
            };
        };
        // Normally 0, 1 for carrying sliding board on spiral slide ride, 2 for carrying lawn mower
        uint8_t SpecialSprite;    // 0x6D
        uint8_t ActionSpriteType; // 0x6E
        // Seems to be used like a local variable, as it's always set before calling SwitchNextActionSpriteType, which reads
        // this again
        uint8_t NextActionSpriteType;    // 0x6F
        uint8_t ActionSpriteImageOffset; // 0x70
        uint8_t Action;                  // 0x71
        uint8_t ActionFrame;             // 0x72
        uint8_t StepProgress;
        union
        {
            uint16_t MechanicTimeSinceCall; // time getting to ride to fix
            uint16_t NextInQueue;           // 0x74
        };
        uint8_t Pad76;
        uint8_t Pad77;
        union
        {
            uint8_t MazeLastEdge; // 0x78
            uint8_t Direction;    // Direction ?
        };
        RCT12RideId InteractionRideIndex;
        uint16_t TimeInQueue;      // 0x7A
        uint8_t rides_been_on[32]; // 0x7C
        // 255 bit bitmap of every ride the peep has been on see
        // WindowPeepRidesUpdate for how to use.
        uint32_t ID;                                        // 0x9C
        money32 CashInPocket;                               // 0xA0
        money32 CashSpent;                                  // 0xA4
        int32_t ParkEntryTime;                              // 0xA8
        int8_t RejoinQueueTimeout;                          // 0xAC
        RCT12RideId PreviousRide;                           // 0xAD
        uint16_t PreviousRideTimeOut;                       // 0xAE
        RCT12PeepThought Thoughts[Limits::MaxPeepThoughts]; // 0xB0
        uint8_t PadC4;
        union
        {
            uint8_t StaffID;                  // 0xC5
            RCT12RideId GuestHeadingToRideID; // 0xC5
        };
        union
        {
            uint8_t StaffOrders;         // 0xC6
            uint8_t PeepIsLostCountdown; // 0xC6
        };
        RCT12RideId Photo1RideRef;      // 0xC7
        uint32_t PeepFlags;             // 0xC8
        rct12_xyzd8 PathfindGoal;       // 0xCC
        rct12_xyzd8 PathfindHistory[4]; // 0xD0
        uint8_t NoActionFrameNum;       // 0xE0
        // 0x3F Litter Count split into lots of 3 with time, 0xC0 Time since last recalc
        uint8_t LitterCount; // 0xE1
        union
        {
            uint8_t TimeOnRide;         // 0xE2
            uint8_t StaffMowingTimeout; // 0xE2
        };
        // 0x3F Sick Count split into lots of 3 with time, 0xC0 Time since last recalc
        uint8_t DisgustingCount;            // 0xE3
        money16 PaidToEnter;                // 0xE4
        money16 PaidOnRides;                // 0xE6
        money16 PaidOnFood;                 // 0xE8
        money16 PaidOnSouvenirs;            // 0xEA
        uint8_t NumFood;                    // 0xEC
        uint8_t NumDrinks;                  // 0xED
        uint8_t NumSouvenirs;               // 0xEE
        uint8_t VandalismSeen;              // 0xEF
        uint8_t VoucherType;                // 0xF0
        RCT12RideId VoucherArguments;       // 0xF1 RideID or StringOffsetID
        uint8_t SurroundingsThoughtTimeout; // 0xF2
        uint8_t Angriness;                  // 0xF3
        uint8_t TimeLost;
        uint8_t DaysInQueue;         // 0xF5
        uint8_t BalloonColour;       // 0xF6
        uint8_t UmbrellaColour;      // 0xF7
        uint8_t HatColour;           // 0xF8
        RCT12RideId FavouriteRide;   // 0xF9
        uint8_t FavouriteRideRating; // 0xFA
        uint8_t PadFB;
        uint32_t ItemStandardFlags; // 0xFC
        uint64_t GetItemFlags() const
        {
            return ItemStandardFlags;
        }
    };
    assert_struct_size(Peep, 0x100);

    union Entity
    {
        uint8_t Pad00[0x100];
        UnkEntity Unknown;
        RCT1::Vehicle Vehicle;
        RCT1::Peep Peep;
        RCT12EntityLitter Litter;
        RCT12EntityBalloon Balloon;
        RCT12EntityDuck Duck;
        RCT12EntityJumpingFountain JumpingFountain;
        RCT12EntityMoneyEffect MoneyEffect;
        RCT12EntityCrashedVehicleParticle CrashedVehicleParticle;
        RCT12EntityCrashSplash CrashSplash;
        RCT12EntitySteamParticle SteamParticle;
    };
    assert_struct_size(Entity, 0x100);

    struct ResearchItem
    {
        uint8_t Item;
        uint8_t RelatedRide;
        uint8_t Type;
        uint8_t Flags;
        uint8_t Category;
    };
    assert_struct_size(ResearchItem, 5);

    /**
     * RCT1,AA,LL scenario / saved game structure.
     * size: 0x1F850C
     */
    struct S4
    {
        uint16_t Month;
        uint16_t Day;
        uint32_t Ticks;
        uint32_t RandomA;
        uint32_t RandomB;
        RCT12TileElement TileElements[Limits::MaxTileElements];
        uint32_t UnkCounter;
        Entity Entities[Limits::MaxEntities];
        uint16_t NextEntityIndex;
        uint16_t FirstVehicleEntityIndex;
        uint16_t FirstPeepEntityIndex;
        uint16_t FirstDuckEntityIndex;
        uint16_t FirstLitterEntityIndex;
        uint16_t FirstOversizedRideCarEntityIndex;
        uint16_t EntitiesAvailable;
        uint16_t NumVehicleEntities;
        uint16_t NumPeepEntities;
        uint16_t NumDuckEntities;
        uint16_t NumLitterEntities;
        uint16_t NumOversizedRideCarEntities;
        uint32_t ParkNameStringIndex;
        uint32_t Unk198830;
        money32 Cash;
        money32 Loan;
        uint32_t ParkFlags;
        money16 ParkEntranceFee;
        Entrance ParkEntrance;
        uint8_t Unk198849;
        rct12_peep_spawn PeepSpawn[Limits::MaxPeepSpawns];
        uint8_t Unk198856;
        uint8_t ResearchLevel;
        uint32_t Unk198858;
        uint32_t AvailableRides[8];
        uint32_t AvailableVehicles[8];
        uint32_t RideFeature1[128];
        uint32_t RideFeature2[128];
        uint16_t GuestsInPark;
        uint16_t Unk198C9E;
        money32 Expenditure[Limits::ExpenditureTableMonthCount][Limits::ExpenditureTypeCount];
        uint32_t GuestsInPark2;
        uint8_t Unk199024;
        colour_t HandymanColour;
        colour_t MechanicColour;
        colour_t SecurityGuardColour;
        uint8_t AvailableScenery[128];
        uint16_t AvailableBanners;
        uint8_t Unk1990AA[94];
        uint16_t ParkRating;
        uint8_t ParkRatingHistory[32];
        uint8_t GuestsInParkHistory[32];
        uint8_t ResearchPriority;
        uint8_t ResearchProgressStage;
        uint8_t LastResearchItem;
        uint8_t LastResearchRide;
        uint8_t LastResearchType;
        uint8_t LastResearchFlags;
        ResearchItem ResearchItems[200];
        uint8_t NextResearchItem;
        uint8_t NextResearchRide;
        uint8_t NextResearchType;
        uint8_t NextResearchFlags;
        uint16_t ResearchProgress;
        uint8_t NextResearchCategory;
        uint8_t NextResearchExpectedDay;
        uint8_t NextResearchExpectedMonth;
        uint8_t GuestInitialHappiness;
        uint16_t ParkSize;
        uint16_t GuestGenerationProbability;
        money16 TotalRideValueForMoney;
        money32 MaxLoan;
        money16 GuestInitialCash;
        uint8_t GuestInitialHunger;
        uint8_t GuestInitialThirst;
        uint8_t ScenarioObjectiveType;
        uint8_t ScenarioObjectiveYears;
        uint16_t Unk199552;
        money32 ScenarioObjectiveCurrency;
        uint16_t ScenarioObjectiveNumGuests;
        uint8_t MarketingStatus[20];
        uint8_t MarketingAssoc[20];
        uint8_t Unk199582[2];
        money32 CashHistory[Limits::FinanceGraphSize];
        money32 TotalExpenditure;
        money32 Profit;
        uint8_t Unk199788[8];
        money32 WeeklyProfitHistory[Limits::FinanceGraphSize];
        money32 ParkValue;
        money32 ParkValueHistory[Limits::FinanceGraphSize];
        money32 CompletedCompanyValue;
        uint32_t NumAdmissions;
        money32 AdmissionTotalIncome;
        money32 CompanyValue;
        uint8_t ThoughtTimer[16];
        rct12_award Awards[Limits::MaxAwards];
        money16 LandPrice;
        money16 ConstructionRightsPrice;
        uint16_t Unk199BCC;
        uint16_t Unk199BCE;
        uint32_t Unk199BD0;
        char username[64];
        uint32_t GameVersion;
        money32 ObjectiveCompletionCompanyValue;
        uint32_t FinanceChecksum;
        uint16_t NumRides;
        uint16_t CheatDetectionNegNumRides;
        uint16_t CheatDetectionMaxOwnedTiles;
        uint16_t CheatDetectionNegMaxOwnedTiles;
        uint32_t FinanceChecksum3;
        uint32_t ScenarioSlotIndexChecksum;
        char ScenarioWinner[32];
        uint32_t FinanceChecksum2;
        char CopyrightNotice[40];
        uint16_t CheatDetectionSV6SC4[4];
        uint16_t Unk199C84;
        uint16_t Unk199C86;
        uint16_t MapSizeUnits;
        uint16_t MapSizeUnkB;
        uint16_t MapSize;
        uint16_t MapSizeMaxXY;
        uint32_t SamePriceThroughout;
        uint16_t Unk199C94;
        uint8_t Unk199C96[3];
        uint8_t WaterColour;
        uint16_t Unk199C9A;
        ResearchItem ResearchItemsLL[180];
        uint8_t Unk19A020[5468];
        RCT12Banner Banners[Limits::MaxBanners];
        char StringTable[Limits::MaxUserStrings][Limits::MaxUserStringLength];
        uint32_t GameTimeCounter;
        Ride Rides[Limits::MaxRidesInPark];
        uint16_t UnkGameTimeCounter;
        int16_t ViewX;
        int16_t ViewY;
        uint8_t ViewZoom;
        uint8_t ViewRotation;
        RCT12MapAnimation MapAnimations[Limits::MaxAnimatedObjects];
        uint32_t NumMapAnimations;
        uint8_t Unk1CADBC[12];
        uint16_t ScrollingTextStep;
        uint32_t Unk1CADCA;
        uint16_t Unk1CADCE;
        uint8_t Unk1CADD0[116];
        RCT12RideMeasurement RideMeasurements[8];
        uint32_t NextGuestIndex;
        uint16_t GameCounter5;
        uint8_t PatrolAreas[(Limits::MaxStaff + Limits::StaffTypeCount) * Limits::PatrolAreaSize];
        uint8_t StaffModes[Limits::MaxStaff];
        uint8_t Unk1F431E[4];
        uint8_t Unk1F4322[8];
        uint8_t Climate;
        uint8_t Unk1F432B;
        uint16_t ClimateTimer;
        uint8_t Weather;
        uint8_t TargetWeather;
        uint8_t Temperature;
        uint8_t TargetTemperature;
        uint8_t ThunderFrequency;
        uint8_t TargetThunderFrequency;
        uint8_t WeatherGloom;
        uint8_t TargetWeatherGloom;
        uint8_t Rain;
        uint8_t TargetRain;
        rct12_news_item Messages[Limits::MaxNewsItems];
        char ScenarioName[62];
        uint16_t ScenarioSlotIndex;
        uint32_t ScenarioFlags;
        uint8_t Unk1F8358[432];
        uint32_t ExpansionPackChecksum;
    };
    assert_struct_size(S4, 0x1F850C);

    /**
     * Track design structure. Only for base RCT1
     * size: 0x2006
     */
    struct TD4
    {
        RideType Type; // 0x00
        uint8_t VehicleType;
        uint32_t Flags;                                              // 0x02
        uint8_t Mode;                                                // 0x06
        uint8_t VersionAndColourScheme;                              // 0x07 0b0000_VVCC
        RCT12VehicleColour VehicleColours[Limits::MaxTrainsPerRide]; // 0x08
        uint8_t TrackSpineColourV0;                                  // 0x20
        uint8_t TrackRailColourV0;                                   // 0x21
        uint8_t TrackSupportColourV0;                                // 0x22
        uint8_t DepartFlags;                                         // 0x23
        uint8_t NumberOfTrains;                                      // 0x24
        uint8_t NumberOfCarsPerTrain;                                // 0x25
        uint8_t MinWaitingTime;                                      // 0x26
        uint8_t MaxWaitingTime;                                      // 0x27
        union
        {
            uint8_t OperationSetting;
            uint8_t LaunchSpeed;
            uint8_t NumLaps;
            uint8_t MaxPeople;
        };
        int8_t MaxSpeed;              // 0x29
        int8_t AverageSpeed;          // 0x2A
        uint16_t RideLength;          // 0x2B
        uint8_t MaxPositiveVerticalG; // 0x2D
        int8_t MaxNegativeVerticalG;  // 0x2C
        uint8_t MaxLateralG;          // 0x2F
        union
        {
            uint8_t NumInversions; // 0x30
            uint8_t NumHoles;      // 0x30
        };
        uint8_t NumDrops;          // 0x31
        uint8_t HighestDropHeight; // 0x32
        uint8_t Excitement;        // 0x33
        uint8_t Intensity;         // 0x34
        uint8_t Nausea;            // 0x35
        money16 UpkeepCost;        // 0x36
    };

    assert_struct_size(TD4, 0x38);

    /**
     * Track design structure for Added Attractions / Loopy Landscapes
     * size: 0x2006
     */
    struct TD4AA : public TD4
    {
        uint8_t TrackSpineColour[Limits::NumColourSchemes];   // 0x38
        uint8_t TrackRailColour[Limits::NumColourSchemes];    // 0x3C
        uint8_t TrackSupportColour[Limits::NumColourSchemes]; // 0x40
        uint8_t Flags2;                                       // 0x44

        uint8_t Pad45[0x7F]; // 0x45
    };

    assert_struct_size(TD4AA, 0xC4);
#pragma pack(pop)

    enum
    {
        RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN = 0,
        RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN_BACKWARDS,
        RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_INVERTED_COASTER_TRAIN, // Not in RCT2
        RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_CARS,
        RCT1_VEHICLE_TYPE_LADYBIRD_CARS,
        RCT1_VEHICLE_TYPE_STANDUP_ROLLER_COASTER_CARS,
        RCT1_VEHICLE_TYPE_SPINNING_CARS,
        RCT1_VEHICLE_TYPE_SINGLE_PERSON_SWINGING_CHAIRS,
        RCT1_VEHICLE_TYPE_SWANS_PEDAL_BOATS,
        RCT1_VEHICLE_TYPE_LARGE_MONORAIL_TRAIN,
        RCT1_VEHICLE_TYPE_CANOES,
        RCT1_VEHICLE_TYPE_ROWING_BOATS,
        RCT1_VEHICLE_TYPE_STEAM_TRAIN,
        RCT1_VEHICLE_TYPE_WOODEN_MOUSE_CARS,
        RCT1_VEHICLE_TYPE_BUMPER_BOATS,
        RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN_BACKWARDS,
        RCT1_VEHICLE_TYPE_ROCKET_CARS,
        RCT1_VEHICLE_TYPE_HORSES, // Steeplechase
        RCT1_VEHICLE_TYPE_SPORTSCARS,
        RCT1_VEHICLE_TYPE_LYING_DOWN_SWINGING_CARS, // Inverted single-rail
        RCT1_VEHICLE_TYPE_WOODEN_MINE_CARS,
        RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_AIRPLANE_CARS,
        RCT1_VEHICLE_TYPE_SMALL_MONORAIL_CARS,
        RCT1_VEHICLE_TYPE_WATER_TRICYCLES,
        RCT1_VEHICLE_TYPE_LAUNCHED_FREEFALL_CAR,
        RCT1_VEHICLE_TYPE_BOBSLEIGH_CARS,
        RCT1_VEHICLE_TYPE_DINGHIES,
        RCT1_VEHICLE_TYPE_ROTATING_CABIN,
        RCT1_VEHICLE_TYPE_MINE_TRAIN,
        RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS,
        RCT1_VEHICLE_TYPE_CORKSCREW_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_MOTORBIKES,
        RCT1_VEHICLE_TYPE_RACING_CARS,
        RCT1_VEHICLE_TYPE_TRUCKS,
        RCT1_VEHICLE_TYPE_GO_KARTS,
        RCT1_VEHICLE_TYPE_RAPIDS_BOATS,
        RCT1_VEHICLE_TYPE_LOG_FLUME_BOATS,
        RCT1_VEHICLE_TYPE_DODGEMS,
        RCT1_VEHICLE_TYPE_SWINGING_SHIP,
        RCT1_VEHICLE_TYPE_SWINGING_INVERTER_SHIP,
        RCT1_VEHICLE_TYPE_MERRY_GO_ROUND,
        RCT1_VEHICLE_TYPE_FERRIS_WHEEL,
        RCT1_VEHICLE_TYPE_SIMULATOR_POD,
        RCT1_VEHICLE_TYPE_CINEMA_BUILDING,
        RCT1_VEHICLE_TYPE_TOPSPIN_CAR,
        RCT1_VEHICLE_TYPE_SPACE_RINGS,
        RCT1_VEHICLE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER_CAR,
        RCT1_VEHICLE_TYPE_VERTICAL_ROLLER_COASTER_CARS,
        RCT1_VEHICLE_TYPE_CAT_CARS,
        RCT1_VEHICLE_TYPE_TWIST_ARMS_AND_CARS,
        RCT1_VEHICLE_TYPE_HAUNTED_HOUSE_BUILDING,
        RCT1_VEHICLE_TYPE_LOG_CARS,
        RCT1_VEHICLE_TYPE_CIRCUS_TENT,
        RCT1_VEHICLE_TYPE_GHOST_TRAIN_CARS,
        RCT1_VEHICLE_TYPE_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_WOODEN_TWISTER_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_WOODEN_SIDE_FRICTION_CARS,
        RCT1_VEHICLE_TYPE_VINTAGE_CARS,
        RCT1_VEHICLE_TYPE_STEAM_TRAIN_COVERED_CARS,
        RCT1_VEHICLE_TYPE_STAND_UP_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_FLOORLESS_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_STEEL_MOUSE_CARS,
        RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS_ALTERNATIVE,
        RCT1_VEHICLE_TYPE_SUSPENDED_MONORAIL_TRAIN,
        RCT1_VEHICLE_TYPE_HELICOPTER_CARS,
        RCT1_VEHICLE_TYPE_VIRGINIA_REEL_TUBS,
        RCT1_VEHICLE_TYPE_REVERSER_CARS,
        RCT1_VEHICLE_TYPE_GOLFERS,
        RCT1_VEHICLE_TYPE_RIVER_RIDE_BOATS,
        RCT1_VEHICLE_TYPE_FLYING_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_NON_LOOPING_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS,
        RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS_REVERSED,
        RCT1_VEHICLE_TYPE_RESERVED,
        RCT1_VEHICLE_TYPE_ROTODROP_CAR,
        RCT1_VEHICLE_TYPE_FLYING_SAUCERS,
        RCT1_VEHICLE_TYPE_CROOKED_HOUSE_BUILDING,
        RCT1_VEHICLE_TYPE_BICYCLES,
        RCT1_VEHICLE_TYPE_HYPERCOASTER_TRAIN,
        RCT1_VEHICLE_TYPE_4_ACROSS_INVERTED_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_WATER_COASTER_BOATS,
        RCT1_VEHICLE_TYPE_FACEOFF_CARS,
        RCT1_VEHICLE_TYPE_JET_SKIS,
        RCT1_VEHICLE_TYPE_RAFT_BOATS,
        RCT1_VEHICLE_TYPE_AMERICAN_STYLE_STEAM_TRAIN,
        RCT1_VEHICLE_TYPE_AIR_POWERED_COASTER_TRAIN,
        RCT1_VEHICLE_TYPE_SUSPENDED_WILD_MOUSE_CARS, // Inverted Hairpin in RCT2
        RCT1_VEHICLE_TYPE_ENTERPRISE_WHEEL,

        RCT1_VEHICLE_TYPE_COUNT
    };

    enum
    {
        RCT1_TRACK_ELEM_BOOSTER = 100
    };

    enum
    {
        RCT1_RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE = 2,
        RCT1_RIDE_MODE_POWERED_LAUNCH = 3,
    };

    enum
    {
        RCT1_RIDE_DEPART_PLAY_MUSIC = 1 << 5,
    };

    enum
    {
        RCT1_SCENERY_THEME_GENERAL,
        RCT1_SCENERY_THEME_MINE,
        RCT1_SCENERY_THEME_CLASSICAL_ROMAN,
        RCT1_SCENERY_THEME_EGYPTIAN,
        RCT1_SCENERY_THEME_MARTIAN,
        RCT1_SCENERY_THEME_JUMPING_FOUNTAINS, // Single researchable scenery item
        RCT1_SCENERY_THEME_WONDERLAND,
        RCT1_SCENERY_THEME_JURASSIC,
        RCT1_SCENERY_THEME_SPOOKY,
        RCT1_SCENERY_THEME_JUNGLE,
        RCT1_SCENERY_THEME_ABSTRACT,
        RCT1_SCENERY_THEME_GARDEN_CLOCK, // Single researchable scenery item
        RCT1_SCENERY_THEME_SNOW_ICE,
        RCT1_SCENERY_THEME_MEDIEVAL,
        RCT1_SCENERY_THEME_SPACE,
        RCT1_SCENERY_THEME_CREEPY,
        RCT1_SCENERY_THEME_URBAN,
        RCT1_SCENERY_THEME_PAGODA,
    };

    enum
    {
        RCT1_FOOTPATH_TYPE_QUEUE_BLUE,
        RCT1_FOOTPATH_TYPE_QUEUE_RED,
        RCT1_FOOTPATH_TYPE_QUEUE_YELLOW,
        RCT1_FOOTPATH_TYPE_QUEUE_GREEN,

        RCT1_FOOTPATH_TYPE_TARMAC_GREY,
        RCT1_FOOTPATH_TYPE_TARMAC_RED,
        RCT1_FOOTPATH_TYPE_TARMAC_BROWN,
        RCT1_FOOTPATH_TYPE_TARMAC_GREEN,

        RCT1_FOOTPATH_TYPE_DIRT_BROWN,
        RCT1_FOOTPATH_TYPE_DIRT_BLACK,

        RCT1_FOOTPATH_TYPE_CRAZY_PAVING = 12,

        RCT1_FOOTPATH_TYPE_ROAD = 16,

        RCT1_FOOTPATH_TYPE_TILE_BROWN = 20,
        RCT1_FOOTPATH_TYPE_TILE_GREY,
        RCT1_FOOTPATH_TYPE_TILE_RED,
        RCT1_FOOTPATH_TYPE_TILE_GREEN,
    };

    enum
    {
        FOOTPATH_SUPPORTS_WOODEN_TRUSS,
        FOOTPATH_SUPPORTS_WOOD,
        FOOTPATH_SUPPORTS_STEEL,
        FOOTPATH_SUPPORTS_BAMBOO,
    };

    enum
    {
        RCT1_PATH_ADDITION_NONE,
        RCT1_PATH_ADDITION_LAMP_1,
        RCT1_PATH_ADDITION_LAMP_2,
        RCT1_PATH_ADDITION_BIN,
        RCT1_PATH_ADDITION_BENCH,
        RCT1_PATH_ADDITION_JUMPING_FOUNTAIN,
        RCT1_PATH_ADDITION_LAMP_3,
        RCT1_PATH_ADDITION_LAMP_4,
        RCT1_PATH_ADDITION_BROKEN_LAMP_1,
        RCT1_PATH_ADDITION_BROKEN_LAMP_2,
        RCT1_PATH_ADDITION_BROKEN_BIN,
        RCT1_PATH_ADDITION_BROKEN_BENCH,
        RCT1_PATH_ADDITION_BROKEN_LAMP_3,
        RCT1_PATH_ADDITION_BROKEN_LAMP_4,
        RCT1_PATH_ADDITION_JUMPING_SNOW,
    };

    enum
    {
        RCT1_WALL_TYPE_MESH_FENCE = 0,
        RCT1_WALL_TYPE_MESH_FENCE_WITH_GATE = 1,
        RCT1_WALL_TYPE_ROMAN = 2,
        RCT1_WALL_TYPE_EGYPTIAN = 3,
        RCT1_WALL_TYPE_HEDGE = 4,
        RCT1_WALL_TYPE_HEDGE_WITH_GATE = 5,
        RCT1_WALL_TYPE_BLUE_PLAYING_CARDS = 6,
        RCT1_WALL_TYPE_RED_PLAYING_CARDS = 7,
        RCT1_WALL_TYPE_WHITE_RAILING = 8,
        RCT1_WALL_TYPE_WHITE_RAILING_WITH_GATE = 9,
        RCT1_WALL_TYPE_MARTIAN = 10,
        RCT1_WALL_TYPE_GLASS_SMOOTH = 11,
        RCT1_WALL_TYPE_WOODEN_PANEL_FENCE = 12,
        RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_GATE = 13,
        RCT1_WALL_TYPE_WOODEN_POST_FENCE = 14,
        RCT1_WALL_TYPE_RED_WOODEN_POST_FENCE = 15,
        RCT1_WALL_TYPE_BARBED_WIRE = 16,
        RCT1_WALL_TYPE_BARBED_WIRE_WITH_GATE = 17,
        RCT1_WALL_TYPE_PRIMITIVE_TALL_WOOD_FENCE = 18,
        RCT1_WALL_TYPE_PRIMITIVE_SHORT_WOOD_FENCE = 19,
        RCT1_WALL_TYPE_IRON_RAILING = 20,
        RCT1_WALL_TYPE_IRON_RAILING_WITH_GATE = 21,
        RCT1_WALL_TYPE_GLASS_PANELS = 22,
        RCT1_WALL_TYPE_BONE_FENCE = 23,
        RCT1_WALL_TYPE_BRICK = 24,
        RCT1_WALL_TYPE_BRICK_WITH_GATE = 25,
        RCT1_WALL_TYPE_WHITE_WOODEN_PANEL_FENCE = 26,
        RCT1_WALL_TYPE_RED_WOODEN_PANEL_FENCE = 27,
        RCT1_WALL_TYPE_STONE = 28,
        RCT1_WALL_TYPE_STONE_WITH_GATE = 29,
        RCT1_WALL_TYPE_WOODEN_FENCE = 30,
        RCT1_WALL_TYPE_JUNGLE = 31,
        RCT1_WALL_TYPE_CONIFER_HEDGE = 32,
        RCT1_WALL_TYPE_CONIFER_HEDGE_WITH_GATE = 33,
        RCT1_WALL_TYPE_SMALL_BROWN_CASTLE = 34,
        RCT1_WALL_TYPE_SMALL_GREY_CASTLE = 35,
        RCT1_WALL_TYPE_ROMAN_COLUMN = 36,
        RCT1_WALL_TYPE_LARGE_BROWN_CASTLE = 37,
        RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_CROSS = 38,
        RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_GATE = 39,
        RCT1_WALL_TYPE_LARGE_BROWN_CASTLE_WINDOW = 40,
        RCT1_WALL_TYPE_MEDIUM_BROWN_CASTLE = 41,
        RCT1_WALL_TYPE_LARGE_GREY_CASTLE = 42,
        RCT1_WALL_TYPE_LARGE_GREY_CASTLE_CROSS = 43,
        RCT1_WALL_TYPE_LARGE_GREY_CASTLE_GATE = 44,
        RCT1_WALL_TYPE_LARGE_GREY_CASTLE_WINDOW = 45,
        RCT1_WALL_TYPE_MEDIUM_GREY_CASTLE = 46,
        RCT1_WALL_TYPE_CREEPY = 47,
        RCT1_WALL_TYPE_CREEPY_GATE = 48,
        RCT1_WALL_TYPE_BARBED_WIRE_WITH_SNOW = 49,
        RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_SNOW = 50,
        RCT1_WALL_TYPE_WOODEN_POST_FENCE_WITH_SNOW = 51,
    };

    enum
    {
        RCT1_RESEARCH_END_AVAILABLE = 0xFF,
        RCT1_RESEARCH_END_RESEARCHABLE = 0xFE,
        RCT1_RESEARCH_END = 0xFD,
    };

    enum
    {
        RCT1_RESEARCH_TYPE_THEME,
        RCT1_RESEARCH_TYPE_RIDE,
        RCT1_RESEARCH_TYPE_VEHICLE,
        RCT1_RESEARCH_TYPE_SPECIAL,
    };

    enum
    {
        RCT1_RESEARCH_CATEGORY_ROLLERCOASTERS = 1 << 0,
        RCT1_RESEARCH_CATEGORY_THRILL_RIDES = 1 << 1,
        RCT1_RESEARCH_CATEGORY_GENTLE_TRANSPORT_RIDES = 1 << 2,
        RCT1_RESEARCH_CATEGORY_SHOPS = 1 << 3,
        RCT1_RESEARCH_CATEGORY_SCENERY_THEMING = 1 << 4,
        RCT1_RESEARCH_CATEGORY_RIDE_IMPROVEMENTS = 1 << 5,
    };

    // Unconfirmed special track elements for research
    enum
    {
        RCT1_RESEARCH_SPECIAL_BANKED_CURVES = 0x06,
        RCT1_RESEARCH_SPECIAL_VERTICAL_LOOP = 0x07,
        RCT1_RESEARCH_SPECIAL_STEEP_TWIST = 0x0C,
        RCT1_RESEARCH_SPECIAL_INLINE_TWIST = 0x11,
        RCT1_RESEARCH_SPECIAL_HALF_LOOP = 0x12,
        RCT1_RESEARCH_SPECIAL_CORKSCREW = 0x13,
        RCT1_RESEARCH_SPECIAL_BANKED_HELIX_A = 0x15,
        RCT1_RESEARCH_SPECIAL_BANKED_HELIX_B = 0x16,
        RCT1_RESEARCH_SPECIAL_HELIX = 0x17,
        RCT1_RESEARCH_SPECIAL_ON_RIDE_PHOTO = 0x1A,
        RCT1_RESEARCH_SPECIAL_WATER_SPLASH = 0x1B,
        RCT1_RESEARCH_SPECIAL_VERTICAL_DROP = 0x1C,
        RCT1_RESEARCH_SPECIAL_BARREL_ROLL = 0x1D,
        RCT1_RESEARCH_SPECIAL_LAUNCHED_LIFT_HILL = 0x1E,
        RCT1_RESEARCH_SPECIAL_LARGE_LOOP_AND_HALF = 0x1F,
        RCT1_RESEARCH_SPECIAL_REVERSER_TURNTABLE = 0x21,
        RCT1_RESEARCH_SPECIAL_HEARTLINE_ROLL = 0x22,
        RCT1_RESEARCH_SPECIAL_REVERSING_SECTIONS = 0x23,
    };

    enum
    {
        RCT1_SCENARIO_FLAG_0 = 1 << 0,
        RCT1_SCENARIO_FLAG_1 = 1 << 1,
        RCT1_SCENARIO_FLAG_2 = 1 << 2,
        RCT1_SCENARIO_FLAG_3 = 1 << 3,
        RCT1_SCENARIO_FLAG_ENABLE_BANNERS = 1 << 4,
        RCT1_SCENARIO_FLAG_5 = 1 << 5,
        RCT1_SCENARIO_FLAG_6 = 1 << 6,
        RCT1_SCENARIO_FLAG_7 = 1 << 7,
        RCT1_SCENARIO_FLAG_CUSTOM_PARK_ENTRANCE_PATH = 1 << 8,
        RCT1_SCENARIO_FLAG_NO_CASH_RESET = 1 << 9,
        RCT1_SCENARIO_FLAG_10 = 1 << 10,
        RCT1_SCENARIO_FLAG_11 = 1 << 11,
        RCT1_SCENARIO_FLAG_12 = 1 << 12,
        RCT1_SCENARIO_FLAG_CUSTOM_MAP_SIZE = 1 << 13,
        RCT1_SCENARIO_FLAG_14 = 1 << 14,
        RCT1_SCENARIO_FLAG_15 = 1 << 15,
        RCT1_SCENARIO_FLAG_16 = 1 << 16,
        RCT1_SCENARIO_FLAG_17 = 1 << 17,
        RCT1_SCENARIO_FLAG_18 = 1 << 18,
        RCT1_SCENARIO_FLAG_19 = 1 << 19,
    };

    enum
    {
        RCT1_PARK_FLAGS_PARK_OPEN = (1 << 0),
        RCT1_PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT = (1 << 1),
        RCT1_PARK_FLAGS_FORBID_LANDSCAPE_CHANGES = (1 << 2),
        RCT1_PARK_FLAGS_FORBID_TREE_REMOVAL = (1 << 3),
        RCT1_PARK_FLAGS_SHOW_REAL_GUEST_NAMES = (1 << 4),
        RCT1_PARK_FLAGS_FORBID_HIGH_CONSTRUCTION = (1 << 5), // Below tree height
        RCT1_PARK_FLAGS_PREF_LESS_INTENSE_RIDES = (1 << 6),
        RCT1_PARK_FLAGS_FORBID_MARKETING_CAMPAIGN = (1 << 7),
        RCT1_PARK_FLAGS_ANTI_CHEAT_DEPRECATED = (1 << 8), // Not used anymore, used for cheat detection
        RCT1_PARK_FLAGS_PREF_MORE_INTENSE_RIDES = (1 << 9),
        RCT1_PARK_FLAGS_NO_MONEY = (1 << 11), // Used for both scenarios and saved games, unlike RCT2
        RCT1_PARK_FLAGS_DIFFICULT_GUEST_GENERATION = (1 << 12),
        RCT1_PARK_FLAGS_PARK_ENTRY_LOCKED_AT_FREE = (1 << 13), // Off: rides and park entry chargeable. On: only rides
                                                               // chargeable.
        RCT1_PARK_FLAGS_DIFFICULT_PARK_RATING = (1 << 14),
        RCT1_PARK_FLAGS_LOCK_REAL_NAMES_OPTION_DEPRECATED = (1 << 15), // Deprecated now we use a persistent 'real names'
                                                                       // setting
    };

    enum
    {
        STEEL_RC_FRONT = 0,
        STEEL_RC_CARRIAGE = 1,
        WOODEN_RC_TRAIN = 2,
        MONORAIL_CARRIAGE = 10,
        MONORAIL_FRONT = 11,
        MONORAIL_BACK = 12,
        MINIATURE_RAILWAY_TENDER = 15,
        MINIATURE_RAILWAY_LOCOMOTIVE = 16,
        MINIATURE_RAILWAY_CARRIAGE = 17,
        WOODEN_RC_TRAIN_BACKWARDS = 20,
        STEEL_RC_REVERSED_FRONT = 23,
        STEEL_RC_REVERSED_CARRIAGE = 24,
        MINE_TRAIN_FRONT = 35,
        MINE_TRAIN_CARRIAGE = 36,
        CORKSCREW_RC_FRONT = 38,
        CORKSCREW_RC_CARRIAGE = 39,
        LOG_FLUME_BOAT = 45,

        LOG_FLUME_BOAT_REVERSED = 61,
        GHOST_TRAIN_CAR = 63,
        TWISTER_RC_SPOILER = 64,
        TWISTER_RC_CARRIAGE = 65,
        GHOST_TRAIN_INVISIBLE = 66,
        ARTICULATED_RC_FRONT = 67,
        ARTICULATED_RC_CARRIAGE = 68,
        MINIATURE_RAILWAY_CARRIAGE_COVERED = 71,
        STANDUP_TWISTER_RC_CARRIAGE = 72,
        REVERSER_RC_CAR = 79,
        REVERSER_RC_BOGIE = 80,
        MINIGOLF_PLAYER = 81,
        MINIGOLF_BALL = 82,
        SPLASH_BOAT = 83,
        SPLASH_BOAT_INVISIBLE = 84,
        HEARTLINE_TWISTER_FORWARDS = 88,  // Used for both regular cars and “starting reversed“ cars that have been reversed
                                          // again.
        HEARTLINE_TWISTER_BACKWARDS = 89, // Used for both regular cars that went through a reverser, as well as reversed cars
                                          // in the starting position.
        REVERSER_RC_CAR_REVERSED = 95,
        HYPERCOASTER_FRONT = 96,
        HYPERCOASTER_CARRIAGE = 97,
        INVERTED_4_ACROSS_CARRIAGE = 98,
        WATER_COASTER_BOAT = 99,
        WATER_COASTER_INVISIBLE = 101,
        RIVER_RAFT = 103,
        MINIATURE_RAILWAY_AMERICAN_TENDER = 104,
        MINIATURE_RAILWAY_AMERICAN_LOCOMOTIVE = 105,
        AIR_POWERED_VERTICAL_COASTER_TRAIN_FRONT = 106,
        AIR_POWERED_VERTICAL_COASTER_TRAIN_CAR = 107,
    };

    enum
    {
        COPY_COLOUR_1 = -1,
        COPY_COLOUR_2 = -2,
    };

    enum
    {
        RCT1_WATER_CYAN,
        RCT1_WATER_ORANGE
    };

    enum
    {
        RCT1_SCENERY_TULIPS_1 = 65,
        RCT1_SCENERY_TULIPS_2 = 68,

        RCT1_SCENERY_GEOMETRIC_SCULPTURE_1 = 157, // TGE1
        RCT1_SCENERY_GEOMETRIC_SCULPTURE_2 = 162, // TGE2
        RCT1_SCENERY_GEOMETRIC_SCULPTURE_3 = 168, // TGE3
        RCT1_SCENERY_GEOMETRIC_SCULPTURE_4 = 170, // TGE4
        RCT1_SCENERY_GEOMETRIC_SCULPTURE_5 = 171, // TGE5

        RCT1_SCENERY_SMALL_RED_GARDENS = 176, // TG19
    };

    enum
    {
        RCT1_LANDSCAPE_DOOR_CLOSED = 0,
        RCT1_LANDSCAPE_DOOR_HALF_OPEN = 2,
        RCT1_LANDSCAPE_DOOR_OPEN = 3,
    };

    enum
    {
        RCT1_PATH_SUPPORT_TYPE_TRUSS,
        RCT1_PATH_SUPPORT_TYPE_COATED_WOOD,
        RCT1_PATH_SUPPORT_TYPE_SPACE,
        RCT1_PATH_SUPPORT_TYPE_BAMBOO,
    };

    track_type_t RCT1TrackTypeToOpenRCT2(RCT12TrackType origTrackType, ride_type_t rideType);
} // namespace RCT1
