/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/FileSystem.hpp"
#include "../rct12/RCT12.h"
#include "../ride/RideRatings.h"
#include "Limits.h"

#include <tuple>
#include <vector>

struct RideObjectEntry;
class ObjectList;
enum class EditorStep : uint8_t;

namespace RCT2
{
    constexpr const StringId RCT2_RIDE_STRING_START = 2;

    // clang-format off
    constexpr const uint16_t RCT2_OBJECT_ENTRY_COUNT =
        Limits::MaxRideObjects +
        Limits::MaxSmallSceneryObjects +
        Limits::MaxLargeSceneryObjects +
        Limits::MaxWallSceneryObjects +
        Limits::MaxBannerObjects +
        Limits::MaxPathObjects +
        Limits::MaxPathAdditionObjects +
        Limits::MaxSceneryGroupObjects +
        Limits::MaxParkEntranceObjects +
        Limits::MaxWaterObjects +
        Limits::MaxScenarioTextObjects;
    // clang-format on
    static_assert(RCT2_OBJECT_ENTRY_COUNT == 721);

    // clang-format off
    constexpr const int32_t RCT2ObjectEntryGroupCounts[] = {
        Limits::MaxRideObjects,
        Limits::MaxSmallSceneryObjects,
        Limits::MaxLargeSceneryObjects,
        Limits::MaxWallSceneryObjects,
        Limits::MaxBannerObjects,
        Limits::MaxPathObjects,
        Limits::MaxPathAdditionObjects,
        Limits::MaxSceneryGroupObjects,
        Limits::MaxParkEntranceObjects,
        Limits::MaxWaterObjects,
        Limits::MaxScenarioTextObjects,
    };
    // clang-format on

    enum class EntityListId : uint8_t
    {
        Count = 6,
    };
#pragma pack(push, 1)
    /**
     * Ride structure.
     * size: 0x0260
     */
    struct Ride
    {
        uint8_t Type; // 0x000
        // pointer to static info. for example, wild mouse type is 0x36, subtype is
        // 0x4c.
        RCT12ObjectEntryIndex Subtype;                               // 0x001
        uint16_t Pad002;                                             // 0x002
        uint8_t Mode;                                                // 0x004
        uint8_t ColourSchemeType;                                    // 0x005
        RCT12VehicleColour VehicleColours[Limits::MaxTrainsPerRide]; // 0x006
        uint8_t Pad046[0x03]; // 0x046, Used to be track colours in RCT1 without expansions
        // 0 = closed, 1 = open, 2 = test
        uint8_t Status; // 0x049
        StringId Name;  // 0x04A
        union
        {
            uint32_t NameArguments; // 0x04C
            struct
            {
                StringId NameArgumentsTypeName; // 0x04C
                uint16_t NameArgumentsNumber;   // 0x04E
            };
        };
        RCT12xy8 OverallView;                               // 0x050
        RCT12xy8 StationStarts[Limits::MaxStationsPerRide]; // 0x052
        uint8_t StationHeights[Limits::MaxStationsPerRide]; // 0x05A
        uint8_t StationLength[Limits::MaxStationsPerRide];  // 0x05E
        uint8_t StationDepart[Limits::MaxStationsPerRide];  // 0x062
        // ride->vehicle index for current train waiting for passengers
        // at station
        uint8_t TrainAtStation[Limits::MaxStationsPerRide];   // 0x066
        RCT12xy8 Entrances[Limits::MaxStationsPerRide];       // 0x06A
        RCT12xy8 Exits[Limits::MaxStationsPerRide];           // 0x072
        uint16_t LastPeepInQueue[Limits::MaxStationsPerRide]; // 0x07A
        uint8_t Pad082[Limits::MaxStationsPerRide];           // 0x082, Used to be number of peeps in queue in RCT1, but this
                                                              // has moved.
        uint16_t Vehicles[Limits::MaxTrainsPerRide];          // 0x086, Points to the first car in the train
        uint8_t DepartFlags;                                  // 0x0C6

        // Not sure if these should be uint or sint.
        uint8_t NumStations;             // 0x0C7
        uint8_t NumTrains;               // 0x0C8
        uint8_t NumCarsPerTrain;         // 0x0C9
        uint8_t ProposedNumTrains;       // 0x0CA
        uint8_t ProposedNumCarsPerTrain; // 0x0CB
        uint8_t MaxTrains;               // 0x0CC
        uint8_t MinMaxCarsPerTrain;      // 0x0CD
        uint8_t MinWaitingTime;          // 0x0CE
        uint8_t MaxWaitingTime;          // 0x0CF
        union
        {
            uint8_t OperationOption; // 0x0D0
            uint8_t TimeLimit;       // 0x0D0
            uint8_t NumLaps;         // 0x0D0
            uint8_t LaunchSpeed;     // 0x0D0
            uint8_t Speed;           // 0x0D0
            uint8_t Rotations;       // 0x0D0
        };

        uint8_t BoatHireReturnDirection; // 0x0D1
        RCT12xy8 BoatHireReturnPosition; // 0x0D2
        uint8_t MeasurementIndex;        // 0x0D4
        // bits 0 through 4 are the number of helix sections
        // bit 5: spinning tunnel, water splash, or rapids
        // bit 6: log reverser, waterfall
        // bit 7: whirlpool
        uint8_t SpecialTrackElements; // 0x0D5
        uint8_t Pad0D6[2];            // 0x0D6
        // Divide this value by 29127 to get the human-readable max speed
        // (in RCT2, display_speed = (max_speed * 9) >> 18)
        int32_t MaxSpeed;                           // 0x0D8
        int32_t AverageSpeed;                       // 0x0DC
        uint8_t CurrentTestSegment;                 // 0x0E0
        uint8_t AverageSpeedTestTimeout;            // 0x0E1
        uint8_t Pad0E2[0x2];                        // 0x0E2
        int32_t Length[Limits::MaxStationsPerRide]; // 0x0E4
        uint16_t Time[Limits::MaxStationsPerRide];  // 0x0F4
        fixed16_2dp MaxPositiveVerticalG;           // 0x0FC
        fixed16_2dp MaxNegativeVerticalG;           // 0x0FE
        fixed16_2dp MaxLateralG;                    // 0x100
        fixed16_2dp PreviousVerticalG;              // 0x102
        fixed16_2dp PreviousLateralG;               // 0x104
        uint8_t Pad106[0x2];                        // 0x106
        uint32_t TestingFlags;                      // 0x108
        // x y map location of the current track piece during a test
        // this is to prevent counting special tracks multiple times
        RCT12xy8 CurTestTrackLocation; // 0x10C
        // Next 3 variables are related (XXXX XYYY ZZZa aaaa)
        uint16_t TurnCountDefault; // 0x10E X = current turn count
        uint16_t TurnCountBanked;  // 0x110
        uint16_t TurnCountSloped;  // 0x112 X = number turns > 3 elements
        union
        {
            uint8_t Inversions; // 0x114 (???X XXXX)
            uint8_t Holes;      // 0x114 (???X XXXX)
            // This is a very rough approximation of how much of the ride is undercover.
            // It reaches the maximum value of 7 at about 50% undercover and doesn't increase beyond that.
            uint8_t ShelteredEighths; // 0x114 (XXX?-????)
        };
        // Y is number of powered lifts, X is drops
        uint8_t Drops;             // 0x115 (YYXX XXXX)
        uint8_t StartDropHeight;   // 0x116
        uint8_t HighestDropHeight; // 0x117
        int32_t ShelteredLength;   // 0x118
        // Unused always 0? Should affect nausea
        uint16_t Var11C;              // 0x11C
        uint8_t NumShelteredSections; // 0x11E (?abY YYYY)
        // see CurTestTrackLocation
        uint8_t CurTestTrackZ; // 0x11F
        // Customer counter in the current 960 game tick (about 30 seconds) interval
        uint16_t CurNumCustomers; // 0x120
        // Counts ticks to update customer intervals, resets each 960 game ticks.
        uint16_t NumCustomersTimeout; // 0x122
        // Customer count in the last 10 * 960 game ticks (sliding window)
        uint16_t NumCustomers[Limits::CustomerHistorySize]; // 0x124
        money16 Price;                                      // 0x138
        RCT12xy8 ChairliftBullwheelLocation[2];             // 0x13A
        uint8_t ChairliftBullwheelZ[2];                     // 0x13E
        union
        {
            RatingTuple Ratings; // 0x140
            struct
            {
                ride_rating Excitement; // 0x140
                ride_rating Intensity;  // 0x142
                ride_rating Nausea;     // 0x144
            };
        };
        uint16_t Value;                      // 0x146
        uint16_t ChairliftBullwheelRotation; // 0x148
        uint8_t Satisfaction;                // 0x14A
        uint8_t SatisfactionTimeOut;         // 0x14B
        uint8_t SatisfactionNext;            // 0x14C
        // Various flags stating whether a window needs to be refreshed
        uint8_t WindowInvalidateFlags; // 0x14D
        uint8_t Pad14E[0x02];          // 0x14E
        uint32_t TotalCustomers;       // 0x150
        money32 TotalProfit;           // 0x154
        uint8_t Popularity;            // 0x158
        uint8_t PopularityTimeOut;     // 0x159 Updated every purchase and ?possibly by time?
        uint8_t PopularityNext;        // 0x15A When timeout reached this will be the next popularity
        uint8_t NumRiders;             // 0x15B
        uint8_t MusicTuneId;           // 0x15C
        uint8_t SlideInUse;            // 0x15D
        union
        {
            uint16_t SlidePeep; // 0x15E
            uint16_t MazeTiles; // 0x15E
        };
        uint8_t Pad160[0xE];            // 0x160
        uint8_t SlidePeepTShirtColour;  // 0x16E
        uint8_t Pad16F[0x7];            // 0x16F
        uint8_t SpiralSlideProgress;    // 0x176
        uint8_t Pad177[0x9];            // 0x177
        int16_t BuildDate;              // 0x180
        money16 UpkeepCost;             // 0x182
        uint16_t RaceWinner;            // 0x184
        uint8_t Pad186[0x02];           // 0x186
        uint32_t MusicPosition;         // 0x188
        uint8_t BreakdownReasonPending; // 0x18C
        uint8_t MechanicStatus;         // 0x18D
        uint16_t Mechanic;              // 0x18E
        uint8_t InspectionStation;      // 0x190
        uint8_t BrokenVehicle;          // 0x191
        uint8_t BrokenCar;              // 0x192
        uint8_t BreakdownReason;        // 0x193
        money16 PriceSecondary;         // 0x194
        union
        {
            struct
            {
                uint8_t ReliabilitySubvalue;   // 0x196, 0 - 255, acts like the decimals for reliability_percentage
                uint8_t ReliabilityPercentage; // 0x197, Starts at 100 and decreases from there.
            };
            uint16_t Reliability; // 0x196
        };
        // Small constant used to increase the unreliability as the game continues,
        // making breakdowns more and more likely.
        uint8_t UnreliabilityFactor; // 0x198
        // Range from [0, 100]
        uint8_t Downtime;                                     // 0x199
        uint8_t InspectionInterval;                           // 0x19A
        uint8_t LastInspection;                               // 0x19B
        uint8_t DowntimeHistory[Limits::DowntimeHistorySize]; // 0x19C
        uint32_t NoPrimaryItemsSold;                          // 0x1A4
        uint32_t NoSecondaryItemsSold;                        // 0x1A8
        uint8_t BreakdownSoundModifier;                       // 0x1AC
        // Used to oscillate the sound when ride breaks down.
        // 0 = no change, 255 = max change
        uint8_t NotFixedTimeout;                                  // 0x1AD
        uint8_t LastCrashType;                                    // 0x1AE
        uint8_t ConnectedMessageThrottle;                         // 0x1AF
        money32 IncomePerHour;                                    // 0x1B0
        money32 Profit;                                           // 0x1B4
        uint8_t QueueTime[Limits::MaxStationsPerRide];            // 0x1B8
        uint8_t TrackColourMain[Limits::NumColourSchemes];        // 0x1BC
        uint8_t TrackColourAdditional[Limits::NumColourSchemes];  // 0x1C0
        uint8_t TrackColourSupports[Limits::NumColourSchemes];    // 0x1C4
        uint8_t Music;                                            // 0x1C8
        uint8_t EntranceStyle;                                    // 0x1C9
        uint16_t VehicleChangeTimeout;                            // 0x1CA
        uint8_t NumBlockBrakes;                                   // 0x1CC
        uint8_t LiftHillSpeed;                                    // 0x1CD
        uint16_t GuestsFavourite;                                 // 0x1CE
        uint32_t LifecycleFlags;                                  // 0x1D0
        uint8_t VehicleColoursExtended[Limits::MaxTrainsPerRide]; // 0x1D4
        uint16_t TotalAirTime;                                    // 0x1F4
        uint8_t CurrentTestStation;                               // 0x1F6
        uint8_t NumCircuits;                                      // 0x1F7
        int16_t CableLiftX;                                       // 0x1F8
        int16_t CableLiftY;                                       // 0x1FA
        uint8_t CableLiftZ;                                       // 0x1FC
        uint8_t Pad1FD;                                           // 0x1FD
        uint16_t CableLift;                                       // 0x1FE
        uint16_t QueueLength[Limits::MaxStationsPerRide];         // 0x200
        uint8_t Pad208[0x58];                                     // 0x208

        uint8_t GetMinCarsPerTrain() const;
        uint8_t GetMaxCarsPerTrain() const;
        void SetMinCarsPerTrain(uint8_t newValue);
        void SetMaxCarsPerTrain(uint8_t newValue);
    };
    assert_struct_size(Ride, 0x260);

    /* Track Entrance entry size: 0x06 */
    struct TD6EntranceElement
    {
        int8_t z;          // 0x00
        uint8_t direction; // 0x01
        int16_t x;         // 0x02
        int16_t y;         // 0x04
    };
    assert_struct_size(TD6EntranceElement, 0x06);

    /* Track Scenery entry  size: 0x16 */
    struct TD6SceneryElement
    {
        RCTObjectEntry SceneryObject; // 0x00
        int8_t x;                     // 0x10
        int8_t y;                     // 0x11
        int8_t z;                     // 0x12
        uint8_t Flags;                // 0x13 direction quadrant tertiary colour
        uint8_t PrimaryColour;        // 0x14
        uint8_t SecondaryColour;      // 0x15
    };
    assert_struct_size(TD6SceneryElement, 0x16);

    /**
     * Track design structure.
     * size: 0xA3
     */
    struct TD6Track
    {
        uint8_t Type; // 0x00
        RCT12ObjectEntryIndex VehicleType;
        union
        {
            // After loading the track this is converted to
            // a cost but before its a flags register
            money32 Cost;   // 0x02
            uint32_t Flags; // 0x02
        };
        union
        {
            // After loading the track this is converted to
            // a flags register
            uint8_t RideMode;   // 0x06
            uint8_t TrackFlags; // 0x06
        };
        uint8_t VersionAndColourScheme;                              // 0x07 0b0000_VVCC
        RCT12VehicleColour VehicleColours[Limits::MaxTrainsPerRide]; // 0x08
        union
        {
            uint8_t Pad48;
            uint8_t TrackSpineColourRCT1; // 0x48
        };
        union
        {
            uint8_t EntranceStyle;       // 0x49
            uint8_t TrackRailColourRCT1; // 0x49
        };
        union
        {
            uint8_t TotalAirTime;           // 0x4A
            uint8_t TrackSupportColourRCT1; // 0x4A
        };
        uint8_t DepartFlags;          // 0x4B
        uint8_t NumberOfTrains;       // 0x4C
        uint8_t NumberOfCarsPerTrain; // 0x4D
        uint8_t MinWaitingTime;       // 0x4E
        uint8_t MaxWaitingTime;       // 0x4F
        uint8_t OperationSetting;
        int8_t MaxSpeed;              // 0x51
        int8_t AverageSpeed;          // 0x52
        uint16_t RideLength;          // 0x53
        uint8_t MaxPositiveVerticalG; // 0x55
        int8_t MaxNegativeVerticalG;  // 0x56
        uint8_t MaxLateralG;          // 0x57
        union
        {
            uint8_t Inversions; // 0x58
            uint8_t Holes;      // 0x58
        };
        uint8_t Drops;                                             // 0x59
        uint8_t HighestDropHeight;                                 // 0x5A
        uint8_t Excitement;                                        // 0x5B
        uint8_t Intensity;                                         // 0x5C
        uint8_t Nausea;                                            // 0x5D
        money16 UpkeepCost;                                        // 0x5E
        uint8_t TrackSpineColour[Limits::NumColourSchemes];        // 0x60
        uint8_t TrackRailColour[Limits::NumColourSchemes];         // 0x64
        uint8_t TrackSupportColour[Limits::NumColourSchemes];      // 0x68
        uint32_t Flags2;                                           // 0x6C
        RCTObjectEntry VehicleObject;                              // 0x70
        uint8_t SpaceRequiredX;                                    // 0x80
        uint8_t SpaceRequiredY;                                    // 0x81
        uint8_t VehicleAdditionalColour[Limits::MaxTrainsPerRide]; // 0x82
        uint8_t LiftHillSpeedNumCircuits;                          // 0xA2 0bCCCL_LLLL
        // 0xA3 (data starts here in file)
    };
    assert_struct_size(TD6Track, 0xA3);

    /**
     * scores.dat file header.
     * size: 0x10
     */
    struct ScoresHeader
    {
        uint32_t Var0;
        uint32_t Var4;
        uint32_t Var8;
        uint32_t ScenarioCount;
    };
    assert_struct_size(ScoresHeader, 0x10);

    /**
     * An entry of scores.dat
     * size: 0x02B0
     */
    struct ScoresEntry
    {
        char Path[256];
        uint8_t Category;
        uint8_t Pad0101[0x1F];
        int8_t ObjectiveType;
        int8_t ObjectiveArg1;
        int32_t objectiveArg2;
        int16_t objectiveArg3;
        char Name[64];
        char Details[256];
        int32_t Flags;
        money32 CompanyValue;
        char CompletedBy[64];
    };
    assert_struct_size(ScoresEntry, 0x02B0);

    struct Vehicle : RCT12EntityBase
    {
        uint8_t Pitch;        // 0x1F
        uint8_t BankRotation; // 0x20
        uint8_t Pad21[3];
        int32_t RemainingDistance;  // 0x24
        int32_t Velocity;           // 0x28
        int32_t Acceleration;       // 0x2C
        uint8_t Ride;               // 0x30
        uint8_t VehicleType;        // 0x31
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
        uint16_t PrevVehicleOnRide;  // 0x40
        uint16_t NextVehicleOnRide;  // 0x42
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
        union
        {
            int16_t SwingSpeed;
            int16_t CrashZ; // 0x4E
        };
        uint8_t Status;                // 0x50
        uint8_t SubState;              // 0x51
        uint16_t Peep[32];             // 0x52
        uint8_t PeepTshirtColours[32]; // 0x92
        uint8_t NumSeats;              // 0xB2
        uint8_t NumPeeps;              // 0xB3
        uint8_t NextFreeSeat;          // 0xB4
        uint8_t RestraintsPosition;    // 0xB5
        union
        {
            int16_t SpinSpeed; // 0xB6
            int16_t CrashX;    // 0xB6
        };
        uint16_t Sound2Flags; // 0xB8
        uint8_t SpinSprite;   // 0xBA
        uint8_t Sound1Id;     // 0xBB
        uint8_t Sound1Volume; // 0xBC
        uint8_t Sound2Id;     // 0xBD
        uint8_t Sound2Volume; // 0xBE
        int8_t SoundVectorFactor;
        union
        {
            uint16_t VarC0;
            int16_t CrashY;           // 0xC0
            uint16_t TimeWaiting;     // 0xC0
            uint16_t CableLiftTarget; // 0xC0
        };
        uint8_t Speed;               // 0xC2
        uint8_t PoweredAcceleration; // 0xC3
        union
        {
            uint8_t DodgemsCollisionDirection; // 0xC4
            uint8_t CollisionDetectionTimer;   // 0xC4
        };
        uint8_t AnimationFrame; // 0xC5
        uint8_t PadC6[0x2];
        uint32_t AnimationState;
        uint8_t ScreamSoundId; // 0xCC
        uint8_t TrackSubposition;
        uint8_t NumLaps;              // 0xCE
        uint8_t BrakeSpeed;           // 0xCF
        uint16_t LostTimeOut;         // 0xD0
        int8_t VerticalDropCountdown; // 0xD1
        uint8_t VarD3;
        uint8_t MiniGolfCurrentAnimation;
        uint8_t MiniGolfFlags;      // 0xD5
        uint8_t RideSubtype;        // 0xD6
        uint8_t ColoursExtended;    // 0xD7
        uint8_t SeatRotation;       // 0xD8
        uint8_t TargetSeatRotation; // 0xD9

        uint16_t GetTrackType() const
        {
            return TrackTypeAndDirection >> 2;
        }
        uint8_t GetTrackDirection() const
        {
            return TrackTypeAndDirection & RCT12VehicleTrackDirectionMask;
        }
        void SetTrackType(uint16_t trackType)
        {
            // set the upper 14 bits to 0
            TrackTypeAndDirection &= ~RCT12VehicleTrackTypeMask;
            TrackTypeAndDirection |= trackType << 2;
        }
        void SetTrackDirection(uint8_t trackDirection)
        {
            // set the lower 2 bits only
            TrackTypeAndDirection &= ~RCT12VehicleTrackDirectionMask;
            TrackTypeAndDirection |= trackDirection & RCT12VehicleTrackDirectionMask;
        }
    };
    assert_struct_size(Vehicle, 0xDA);

    struct Peep : RCT12EntityBase
    {
        uint8_t Pad1F[0x22 - 0x1F];
        StringId NameStringIdx; // 0x22
        uint16_t NextX;         // 0x24
        uint16_t NextY;         // 0x26
        uint8_t NextZ;          // 0x28
        uint8_t NextFlags;      // 0x29
        uint8_t OutsideOfPark;  // 0x2A
        uint8_t State;          // 0x2B
        uint8_t SubState;       // 0x2C
        uint8_t SpriteType;     // 0x2D
        uint8_t PeepType;       // 0x2E
        union
        {
            uint8_t StaffType; // 0x2F
            uint8_t NoOfRides; // 0x2F
        };
        uint8_t TshirtColour;         // 0x30
        uint8_t TrousersColour;       // 0x31
        uint16_t DestinationX;        // 0x32
        uint16_t DestinationY;        // 0x34
        uint8_t DestinationTolerance; // 0x36
        uint8_t Var37;
        uint8_t Energy;                // 0x38
        uint8_t EnergyTarget;          // 0x39
        uint8_t Happiness;             // 0x3A
        uint8_t HappinessTarget;       // 0x3B
        uint8_t Nausea;                // 0x3C
        uint8_t NauseaTarget;          // 0x3D
        uint8_t Hunger;                // 0x3E
        uint8_t Thirst;                // 0x3F
        uint8_t Toilet;                // 0x40
        uint8_t Mass;                  // 0x41
        uint8_t TimeToConsume;         // 0x42
        uint8_t Intensity;             // 0x43
        uint8_t NauseaTolerance;       // 0x44
        uint8_t WindowInvalidateFlags; // 0x45
        money16 PaidOnDrink;           // 0x46
        uint8_t RideTypesBeenOn[16];   // 0x48
        uint32_t ItemExtraFlags;       // 0x58
        RCT12RideId Photo2RideRef;     // 0x5C
        RCT12RideId Photo3RideRef;     // 0x5D
        RCT12RideId Photo4RideRef;     // 0x5E
        uint8_t Pad5F[0x09];           // 0x5F
        RCT12RideId CurrentRide;       // 0x68
        uint8_t CurrentRideStation;    // 0x69
        uint8_t CurrentTrain;          // 0x6A
        union
        {
            struct
            {
                uint8_t CurrentCar;  // 0x6B
                uint8_t CurrentSeat; // 0x6C
            };
            uint16_t TimeToSitdown; // 0x6B
            struct
            {
                uint8_t TimeToStand;   // 0x6B
                uint8_t StandingFlags; // 0x6C
            };
        };
        uint8_t SpecialSprite;           // 0x6D
        uint8_t ActionSpriteType;        // 0x6E
        uint8_t NextActionSpriteType;    // 0x6F
        uint8_t ActionSpriteImageOffset; // 0x70
        uint8_t Action;                  // 0x71
        uint8_t ActionFrame;             // 0x72
        uint8_t StepProgress;            // 0x73
        union
        {
            uint16_t MechanicTimeSinceCall;
            uint16_t NextInQueue; // 0x74
        };
        uint8_t Pad76;
        uint8_t Pad77;
        union
        {
            uint8_t MazeLastEdge; // 0x78
            uint8_t Direction;
        };
        RCT12RideId InteractionRideIndex;
        uint16_t TimeInQueue;                               // 0x7A
        uint8_t RidesBeenOn[32];                            // 0x7C
        uint32_t Id;                                        // 0x9C
        money32 CashInPocket;                               // 0xA0
        money32 CashSpent;                                  // 0xA4
        int32_t ParkEntryTime;                              // 0xA8
        int8_t RejoinQueueTimeout;                          // 0xAC
        RCT12RideId PreviousRide;                           // 0xAD
        uint16_t PreviousRideTimeOut;                       // 0xAE
        RCT12PeepThought Thoughts[Limits::MaxPeepThoughts]; // 0xB0
        uint8_t PathCheckOptimisation;                      // 0xC4
        union
        {
            uint8_t StaffId;                  // 0xC5
            RCT12RideId GuestHeadingToRideId; // 0xC5
        };
        union
        {
            uint8_t StaffOrders;         // 0xC6
            uint8_t PeepIsLostCountdown; // 0xC6
        };
        RCT12RideId Photo1RideRef;     // 0xC7
        uint32_t PeepFlags;            // 0xC8
        RCT12xyzd8 PathfindGoal;       // 0xCC
        RCT12xyzd8 PathfindHistory[4]; // 0xD0
        uint8_t NoActionFrameNum;      // 0xE0
        uint8_t LitterCount;           // 0xE1
        union
        {
            uint8_t TimeOnRide;         // 0xE2
            uint8_t StaffMowingTimeout; // 0xE2
        };
        uint8_t DisgustingCount; // 0xE3
        union
        {
            money16 PaidToEnter;      // 0xE4
            uint16_t StaffLawnsMown;  // 0xE4
            uint16_t StaffRidesFixed; // 0xE4
        };
        union
        {
            money16 PaidOnRides;          // 0xE6
            uint16_t StaffGardensWatered; // 0xE6
            uint16_t StaffRidesInspected; // 0xE6
        };
        union
        {
            money16 PaidOnFood;        // 0xE8
            uint16_t StaffLitterSwept; // 0xE8
        };
        union
        {
            money16 PaidOnSouvenirs;   // 0xEA
            uint16_t StaffBinsEmptied; // 0xEA
        };
        uint8_t NoOfFood;                   // 0xEC
        uint8_t NoOfDrinks;                 // 0xED
        uint8_t NoOfSouvenirs;              // 0xEE
        uint8_t VandalismSeen;              // 0xEF 0xC0 vandalism thought timeout, 0x3F vandalism tiles seen
        uint8_t VoucherType;                // 0xF0
        RCT12RideId VoucherArguments;       // 0xF1 ride_id or string_offset_id
        uint8_t SurroundingsThoughtTimeout; // 0xF2
        uint8_t Angriness;                  // 0xF3
        uint8_t TimeLost;            // 0xF4 the time the peep has been lost when it reaches 254 generates the lost thought
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
            return ItemStandardFlags | (static_cast<uint64_t>(ItemExtraFlags) << 32);
        }
    };
    assert_struct_size(Peep, 0x100);

    enum class StaffMode : uint8_t
    {
        None,
        Walk,
        Patrol = 3
    };

    union Entity
    {
    private:
        uint8_t Pad00[0x100];

    public:
        RCT12EntityBase Unknown;
        RCT2::Vehicle Vehicle;
        RCT2::Peep Peep;
        RCT12EntityLitter Litter;
        RCT12EntityBalloon Balloon;
        RCT12EntityDuck Duck;
        RCT12EntityJumpingFountain JumpingFountain;
        RCT12EntityMoneyEffect MoneyEffect;
        RCT12EntityCrashedVehicleParticle CrashedVehicleParticle;
        RCT12EntityCrashSplash CrashSplash;
        RCT12EntitySteamParticle SteamParticle;
        RCT12EntityParticle MiscParticle;
    };
    assert_struct_size(Entity, 0x100);

    struct RideRatingCalculationData
    {
        uint16_t ProximityX;
        uint16_t ProximityY;
        uint16_t ProximityZ;
        uint16_t ProximityStartX;
        uint16_t ProximityStartY;
        uint16_t ProximityStartZ;
        uint8_t CurrentRide;
        uint8_t State;
        uint8_t ProximityTrackType;
        uint8_t ProximityBaseHeight;
        uint16_t ProximityTotal;
        uint16_t ProximityScores[26];
        uint16_t NumBrakes;
        uint16_t NumReversers;
        uint16_t StationFlags;
    };
    assert_struct_size(RideRatingCalculationData, 76);

    /**
     * SV6/SC6 header chunk
     * size: 0x20
     */
    struct S6Header
    {
        uint8_t Type;              // 0x00
        uint8_t ClassicFlag;       // 0x01
        uint16_t NumPackedObjects; // 0x02
        uint32_t Version;          // 0x04
        uint32_t MagicNumber;      // 0x08
        uint8_t Pad0C[0x14];
    };
    assert_struct_size(S6Header, 0x20);

    /**
     * SC6 information chunk
     * size: 0x198
     */
    struct S6Info
    {
        ::EditorStep EditorStep;
        uint8_t Category;      // 0x01
        uint8_t ObjectiveType; // 0x02
        uint8_t ObjectiveArg1; // 0x03
        int32_t ObjectiveArg2; // 0x04
        int16_t ObjectiveArg3; // 0x08
        uint8_t Pad00A[0x3E];
        char Name[64];        // 0x48
        char Details[256];    // 0x88
        RCTObjectEntry Entry; // 0x188
    };
    assert_struct_size(S6Info, 0x198);

    struct S6Data
    {
        // SC6[0]
        S6Header Header;

        // SC6[1]
        S6Info Info;

        // SC6[2]
        // packed objects

        // SC6[3]
        union
        {
            RCTObjectEntry Objects[RCT2_OBJECT_ENTRY_COUNT];
            struct
            {
                RCTObjectEntry RideObjects[Limits::MaxRideObjects];
                RCTObjectEntry SceneryObjects[Limits::MaxSmallSceneryObjects];
                RCTObjectEntry LargeSceneryObjects[Limits::MaxLargeSceneryObjects];
                RCTObjectEntry WallSceneryObjects[Limits::MaxWallSceneryObjects];
                RCTObjectEntry BannerObjects[Limits::MaxBannerObjects];
                RCTObjectEntry PathObjects[Limits::MaxPathObjects];
                RCTObjectEntry PathAdditionObjects[Limits::MaxPathAdditionObjects];
                RCTObjectEntry SceneryGroupObjects[Limits::MaxSceneryGroupObjects];
                RCTObjectEntry ParkEntranceObjects[Limits::MaxParkEntranceObjects];
                RCTObjectEntry WaterObjects[Limits::MaxWaterObjects];
                RCTObjectEntry ScenarioTextObjects[Limits::MaxScenarioTextObjects];
            };
        };

        // SC6[4]
        uint16_t ElapsedMonths;
        uint16_t CurrentDay;
        uint32_t ScenarioTicks;
        uint32_t ScenarioSrand0;
        uint32_t ScenarioSrand1;

        // SC6[5]
        RCT12TileElement TileElements[Limits::MaxTileElements];

        // SC6[6]
        uint32_t NextFreeTileElementPointerIndex;
        Entity Entities[Limits::MaxEntitiesRCTCExtended];
        uint16_t EntityListsHead[static_cast<uint8_t>(EntityListId::Count)];
        uint16_t EntityListsCount[static_cast<uint8_t>(EntityListId::Count)];
        StringId ParkName;
        uint8_t Pad013573D6[2];
        uint32_t ParkNameArgs;
        money32 InitialCash;
        money32 CurrentLoan;
        uint32_t ParkFlags;
        money16 ParkEntranceFee;
        uint16_t RCT1ParkEntranceX;
        uint16_t RCT1ParkEntranceY;
        uint8_t Pad013573EE[2];
        uint8_t RCT1ParkEntranceZ;
        uint8_t Pad013573F1;
        RCT12PeepSpawn PeepSpawns[Limits::MaxPeepSpawns];
        uint8_t GuestCountChangeModifier;
        uint8_t CurrentResearchLevel;
        uint8_t Pad01357400[4];
        uint32_t ResearchedRideTypes[Limits::MaxResearchedRideTypeQuads];
        uint32_t ResearchedRideEntries[Limits::MaxResearchedRideEntryQuads];
        uint32_t ResearchedTrackTypesA[128];
        uint32_t ResearchedTrackTypesB[128];

        // SC6[7]
        uint16_t GuestsInPark;
        uint16_t GuestsHeadingForPark;

        // Ignored in scenario
        money32 ExpenditureTable[Limits::ExpenditureTableMonthCount][Limits::ExpenditureTypeCount];

        // SC6[8]
        uint16_t LastGuestsInPark;
        uint8_t Pad01357BCA[3];
        uint8_t HandymanColour;
        uint8_t MechanicColour;
        uint8_t SecurityColour;

        // Ignored in scenario
        uint32_t ResearchedSceneryItems[Limits::MaxResearchedSceneryItemQuads];

        // SC6[9]
        uint16_t ParkRating;

        // Ignored in scenario
        uint8_t ParkRatingHistory[32];
        uint8_t GuestsInParkHistory[32];

        // SC6[10]
        uint8_t ActiveResearchTypes;
        uint8_t ResearchProgressStage;
        uint32_t LastResearchedItemSubject;
        uint8_t Pad01357CF8[1000];
        uint32_t NextResearchItem;
        uint16_t ResearchProgress;
        uint8_t NextResearchCategory;
        uint8_t NextResearchExpectedDay;
        uint8_t NextResearchExpectedMonth;
        uint8_t GuestInitialHappiness;
        uint16_t ParkSize;
        uint16_t GuestGenerationProbability;
        uint64_t TotalRideValueForMoney;
        money32 MaximumLoan;
        money16 GuestInitialCash;
        uint8_t GuestInitialHunger;
        uint8_t GuestInitialThirst;
        uint8_t ObjectiveType;
        uint8_t ObjectiveYear;
        uint8_t Pad013580FA[2];
        money32 ObjectiveCurrency;
        uint16_t ObjectiveGuests;
        uint8_t CampaignWeeksLeft[20];
        uint8_t CampaignRideIndex[22];

        // Ignored in scenario
        money32 BalanceHistory[Limits::FinanceGraphSize];

        // SC6[11]
        money32 CurrentExpenditure;
        money32 CurrentProfit;
        money32 WeeklyProfitAverageDividend;
        uint16_t WeeklyProfitAverageDivisor;
        uint8_t Pad0135833A[2];

        // Ignored in scenario
        money32 WeeklyProfitHistory[Limits::FinanceGraphSize];

        // SC6[12]
        money32 ParkValue;

        // Ignored in scenario
        money32 ParkValueHistory[Limits::FinanceGraphSize];

        // SC6[13]
        money32 CompletedCompanyValue;
        uint32_t TotalAdmissions;
        money32 IncomeFromAdmissions;
        money32 CompanyValue;
        uint8_t PeepWarningThrottle[16];
        RCT12Award Awards[Limits::MaxAwards];
        money16 LandPrice;
        money16 ConstructionRightsPrice;
        uint16_t Word01358774;
        uint8_t Pad01358776[2];
        uint32_t CDKey;
        uint8_t Pad0135877C[64];
        uint32_t GameVersionNumber;
        money32 CompletedCompanyValueRecord;
        uint32_t LoanHash;
        uint16_t RideCount;
        uint8_t Pad013587CA[6];
        money32 HistoricalProfit;
        uint8_t Pad013587D4[4];
        char ScenarioCompletedName[32];
        money32 Cash;
        uint8_t Pad013587FC[50];
        uint16_t ParkRatingCasualtyPenalty;
        uint16_t MapSizeUnits;
        uint16_t MapSizeMinus2;
        uint16_t MapSize;
        uint16_t MapMaxXy;
        uint32_t SamePriceThroughout;
        uint16_t SuggestedMaxGuests;
        uint16_t ParkRatingWarningDays;
        uint8_t LastEntranceStyle;
        uint8_t RCT1WaterColour;
        uint8_t Pad01358842[2];
        RCT12ResearchItem ResearchItems[Limits::MaxResearchItems];
        uint16_t MapBaseZ;
        char ScenarioName[64];
        char ScenarioDescription[256];
        uint8_t CurrentInterestRate;
        uint8_t Pad0135934B;
        uint32_t SamePriceThroughoutExtended;
        int16_t ParkEntranceX[Limits::MaxParkEntrances];
        int16_t ParkEntranceY[Limits::MaxParkEntrances];
        int16_t ParkEntranceZ[Limits::MaxParkEntrances];
        uint8_t ParkEntranceDirection[Limits::MaxParkEntrances];
        char ScenarioFilename[256];
        uint8_t SavedExpansionPackNames[3256];
        RCT12Banner Banners[Limits::MaxBanners];
        char CustomStrings[Limits::MaxUserStrings][Limits::MaxUserStringLength];
        uint32_t GameTicks1;
        Ride Rides[Limits::MaxRidesInPark];
        uint16_t SavedAge;
        int16_t SavedViewX;
        int16_t SavedViewY;
        uint8_t SavedViewZoom;
        uint8_t SavedViewRotation;
        RCT12MapAnimation MapAnimations[Limits::MaxAnimatedObjects];
        uint16_t NumMapAnimations;
        uint8_t Pad0138B582[2];
        RideRatingCalculationData RideRatingsCalcData;
        uint8_t Pad0138B5D0[60];
        RCT12RideMeasurement RideMeasurements[8];
        uint32_t NextGuestIndex;
        uint16_t GrassAndSceneryTilepos;
        uint32_t PatrolAreas[(Limits::MaxStaff + Limits::StaffTypeCount) * Limits::PatrolAreaSize];
        StaffMode StaffModes[Limits::MaxStaff + Limits::StaffTypeCount];
        uint8_t Pad13CA73E;
        uint8_t Pad13CA73F;
        uint8_t Byte13CA740;
        uint8_t Pad13CA741;
        uint8_t Byte13CA742[4]; // unused
        uint8_t Climate;
        uint8_t Pad013CA747;
        uint16_t ClimateUpdateTimer;
        uint8_t CurrentWeather;
        uint8_t NextWeather;
        uint8_t Temperature;
        uint8_t NextTemperature;
        uint8_t CurrentWeatherEffect;
        uint8_t NextWeatherEffect;
        uint8_t CurrentWeatherGloom;
        uint8_t NextWeatherGloom;
        uint8_t CurrentWeatherLevel;
        uint8_t NextWeatherLevel;
        RCT12NewsItem NewsItems[Limits::MaxNewsItems];
        char RCT1ScenarioName[62];      // Unused in RCT2
        uint16_t RCT1ScenarioSlotIndex; // Unused in RCT2
        uint32_t RCT1ScenarioFlags;     // Unused in RCT2
        uint16_t WidePathTileLoopX;
        uint16_t WidePathTileLoopY;
        uint8_t Pad13CE778[434];
    };
    assert_struct_size(S6Data, 0x5a3c4a);

    struct StexEntry
    {
        StringId ScenarioName; // 0x00
        StringId ParkName;     // 0x02
        StringId Details;      // 0x04
        uint8_t Var06;
    };
    assert_struct_size(StexEntry, 7);
#pragma pack(pop)

    ObjectEntryIndex RCT2RideTypeToOpenRCT2RideType(uint8_t rct2RideType, const RideObjectEntry& rideEntry);
    bool RCT2TrackTypeIsBooster(ride_type_t rideType, uint16_t trackType);
    bool RCT2RideTypeNeedsConversion(uint8_t rct2RideType);
    uint8_t OpenRCT2RideTypeToRCT2RideType(ObjectEntryIndex openrct2Type);
    track_type_t RCT2TrackTypeToOpenRCT2(RCT12TrackType origTrackType, ride_type_t rideType, bool convertFlat);
    RCT12TrackType OpenRCT2TrackTypeToRCT2(track_type_t origTrackType);

    /**
     * Iterates an RCT2 string buffer and returns the length of the string in bytes.
     * Handles single and multi-byte strings.
     */
    size_t GetRCT2StringBufferLen(const char* buffer, size_t maxBufferLen);

    struct FootpathMapping
    {
        std::string_view Original;
        std::string_view NormalSurface;
        std::string_view QueueSurface;
        std::string_view Railing;
    };

    const FootpathMapping* GetFootpathSurfaceId(
        const ObjectEntryDescriptor& desc, bool ideallyLoaded = false, bool isQueue = false);
    std::optional<RCTObjectEntry> GetBestObjectEntryForSurface(std::string_view surface, std::string_view railings);

    static constexpr std::string_view DefaultTerrainSurfaces[] = {
        "rct2.terrain_surface.grass",        "rct2.terrain_surface.sand",        "rct2.terrain_surface.dirt",
        "rct2.terrain_surface.rock",         "rct2.terrain_surface.martian",     "rct2.terrain_surface.chequerboard",
        "rct2.terrain_surface.grass_clumps", "rct2.terrain_surface.ice",         "rct2.terrain_surface.grid_red",
        "rct2.terrain_surface.grid_yellow",  "rct2.terrain_surface.grid_purple", "rct2.terrain_surface.grid_green",
        "rct2.terrain_surface.sand_red",     "rct2.terrain_surface.sand_brown",
    };

    // Additional surface styles added to OpenRCT2 as a feature if RCT1 linked
    static constexpr std::string_view OpenRCT2HybridTerrainSurfaces[] = {
        "rct1aa.terrain_surface.roof_red",
        "rct1ll.terrain_surface.roof_grey",
        "rct1ll.terrain_surface.rust",
        "rct1ll.terrain_surface.wood",
    };

    static constexpr std::string_view DefaultTerrainEdges[] = {
        "rct2.terrain_edge.rock",
        "rct2.terrain_edge.wood_red",
        "rct2.terrain_edge.wood_black",
        "rct2.terrain_edge.ice",
    };

    // Additional surface edges added to OpenRCT2 as a feature if RCT1 was linked
    static constexpr std::string_view OpenRCT2HybridTerrainEdges[] = {
        "rct1.terrain_edge.brick",          "rct1.terrain_edge.iron",           "rct1aa.terrain_edge.grey",
        "rct1aa.terrain_edge.yellow",       "rct1aa.terrain_edge.red",          "rct1ll.terrain_edge.purple",
        "rct1ll.terrain_edge.green",        "rct1ll.terrain_edge.stone_brown",  "rct1ll.terrain_edge.stone_grey",
        "rct1ll.terrain_edge.skyscraper_a", "rct1ll.terrain_edge.skyscraper_b",
    };
} // namespace RCT2

std::vector<uint8_t> DecryptSea(const fs::path& path);
