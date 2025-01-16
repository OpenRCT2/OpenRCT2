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
#include "../object/Object.h"
#include "../world/Map.h"
#include "../world/QuarterTile.h"

#include <optional>

constexpr uint8_t kRCT2DefaultBlockBrakeSpeed = 2;
constexpr int32_t kBlockBrakeBaseSpeed = 0x20364;
constexpr int32_t kBlockBrakeSpeedOffset = kBlockBrakeBaseSpeed - (kRCT2DefaultBlockBrakeSpeed << 16);

constexpr uint8_t kMaximumTrackSpeed = 30;

namespace OpenRCT2
{
    enum class TrackElemType : uint16_t;
}

struct ResultWithMessage;
struct TileElement;

enum class TrackRoll : uint8_t
{
    None = 0,
    Left = 2,
    Right = 4,
    UpsideDown = 15,
};

enum class TrackPitch : uint8_t
{
    None = 0,
    Up25 = 2,
    Up60 = 4,
    Down25 = 6,
    Down60 = 8,
    Up90 = 10,
    Down90 = 18,

    Tower = 10,
    ReverseFreefall = 10
};

// Vehicle sprite groups required by track groups are defined in ride_entry_get_supported_track_pieces
enum class TrackGroup : uint8_t
{
    flat = 0,
    straight,
    stationEnd,
    liftHill,
    liftHillSteep,
    liftHillCurve,
    flatRollBanking,
    verticalLoop,
    slope,
    slopeSteepDown,
    flatToSteepSlope,
    slopeCurve,
    slopeCurveSteep,
    sBend,
    curveVerySmall,
    curveSmall,
    curve,
    curveLarge,
    twist,
    halfLoop,
    corkscrew,
    tower,
    helixUpBankedHalf,
    helixDownBankedHalf,
    helixUpBankedQuarter,
    helixDownBankedQuarter,
    helixUpUnbankedQuarter,
    helixDownUnbankedQuarter,
    brakes,
    onridePhoto,
    waterSplash,
    slopeVertical,
    barrelRoll,
    poweredLift,
    halfLoopLarge,
    slopeCurveBanked,
    logFlumeReverser,
    heartlineRoll,
    reverser,
    reverseFreefall,
    slopeToFlat,
    blockBrakes,
    slopeRollBanking,
    slopeSteepLong,
    curveVertical,
    liftHillCable,
    liftHillCurved,
    quarterLoop,
    spinningTunnel,
    booster,
    inlineTwistUninverted,
    inlineTwistInverted,
    quarterLoopUninvertedUp,
    quarterLoopUninvertedDown,
    quarterLoopInvertedUp,
    quarterLoopInvertedDown,
    rapids,
    flyingHalfLoopUninvertedUp,
    flyingHalfLoopInvertedDown,

    flatRideBase,

    waterfall,
    whirlpool,
    brakeForDrop,
    corkscrewUninverted,
    corkscrewInverted,
    heartlineTransfer,
    miniGolfHole,
    rotationControlToggle,
    slopeSteepUp,

    corkscrewLarge,
    halfLoopMedium,
    zeroGRoll,
    zeroGRollLarge,

    flyingLargeHalfLoopUninvertedUp,
    flyingLargeHalfLoopInvertedDown,
    flyingLargeHalfLoopUninvertedDown,
    flyingLargeHalfLoopInvertedUp,

    flyingHalfLoopUninvertedDown,
    flyingHalfLoopInvertedUp,

    slopeCurveLarge,
    slopeCurveLargeBanked,

    diagBrakes,
    diagBlockBrakes,
    inclinedBrakes,
    diagBooster,

    diagSlopeSteepLong,
    diveLoop,

    count,
};

struct TrackDefinition
{
    TrackGroup group;
    TrackPitch pitchEnd;
    TrackPitch pitchStart;
    TrackRoll rollEnd;
    TrackRoll rollStart;
    int8_t previewZOffset;
};

struct PitchAndRoll
{
    TrackPitch pitch;
    TrackRoll roll;
};
constexpr bool operator==(const PitchAndRoll& vb1, const PitchAndRoll& vb2)
{
    return vb1.pitch == vb2.pitch && vb1.roll == vb2.roll;
}
constexpr bool operator!=(const PitchAndRoll& vb1, const PitchAndRoll& vb2)
{
    return !(vb1 == vb2);
}

struct SequenceClearance
{
    int16_t x{};
    int16_t y{};
    int16_t z{};
    uint8_t clearanceZ{};
    QuarterTile quarterTile = { 0, 0 };
    uint8_t flags{};
};

/* size 0x0A */
struct TrackCoordinates
{
    int8_t rotationBegin; // 0x00
    int8_t rotationEnd;   // 0x01
    int16_t zBegin;       // 0x02
    int16_t zEnd;         // 0x04
    int16_t x;            // 0x06
    int16_t y;            // 0x08
};

enum
{
    RCT_PREVIEW_TRACK_FLAG_0 = (1 << 0),
    RCT_PREVIEW_TRACK_FLAG_1 = (1 << 1),
    RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL = (1 << 2),
};

constexpr int8_t kMaxStationPlatformLength = 32;
constexpr uint16_t const MAX_TRACK_HEIGHT = 254 * kCoordsZStep;
constexpr uint8_t const DEFAULT_SEAT_ROTATION = 4;

enum class TrackCurve : uint8_t
{
    LeftVerySmall = 5,
    LeftSmall = 3,
    Left = 1,
    LeftLarge = 7,
    None = 0,
    RightLarge = 8,
    Right = 2,
    RightSmall = 4,
    RightVerySmall = 6
};
constexpr const uint8_t kHighestCurveValue = 8;

enum
{
    TRACK_ELEM_FLAG_ONLY_UNDERWATER = (1 << 0),
    TRACK_ELEM_FLAG_TURN_LEFT = (1 << 1),
    TRACK_ELEM_FLAG_TURN_RIGHT = (1 << 2),
    TRACK_ELEM_FLAG_TURN_BANKED = (1 << 3),
    TRACK_ELEM_FLAG_TURN_SLOPED = (1 << 4),
    TRACK_ELEM_FLAG_DOWN = (1 << 5),
    TRACK_ELEM_FLAG_UP = (1 << 6),
    TRACK_ELEM_FLAG_NORMAL_TO_INVERSION = (1 << 7),
    TRACK_ELEM_FLAG_IS_GOLF_HOLE = (1 << 7),
    TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT = (1 << 8),
    TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND = (1 << 9),
    TRACK_ELEM_FLAG_IS_STEEP_UP = (1 << 10), // Used to allow steep backwards lifts on roller coasters that do not allow steep
                                             // forward lift hills
    TRACK_ELEM_FLAG_HELIX = (1 << 11),
    TRACK_ELEM_FLAG_ALLOW_LIFT_HILL = (1 << 12),
    TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT = (1 << 13),
    TRACK_ELEM_FLAG_INVERSION_TO_NORMAL = (1 << 14),
    TRACK_ELEM_FLAG_BANKED = (1 << 15), // Also set on Spinning Tunnel and Log Flume reverser, probably to save a flag.
    TRACK_ELEM_FLAG_CAN_BE_PARTLY_UNDERGROUND = (1 << 16),
};

namespace OpenRCT2
{
    enum class TrackElemType : uint16_t
    {
        Flat = 0,
        EndStation = 1,
        BeginStation = 2,
        MiddleStation = 3,
        Up25 = 4,
        Up60 = 5,
        FlatToUp25 = 6,
        Up25ToUp60 = 7,
        Up60ToUp25 = 8,
        Up25ToFlat = 9,
        Down25 = 10,
        Down60 = 11,
        FlatToDown25 = 12,
        Down25ToDown60 = 13,
        Down60ToDown25 = 14,
        Down25ToFlat = 15,
        LeftQuarterTurn5Tiles = 16,
        RightQuarterTurn5Tiles = 17,
        FlatToLeftBank = 18,
        FlatToRightBank = 19,
        LeftBankToFlat = 20,
        RightBankToFlat = 21,
        BankedLeftQuarterTurn5Tiles = 22,
        BankedRightQuarterTurn5Tiles = 23,
        LeftBankToUp25 = 24,
        RightBankToUp25 = 25,
        Up25ToLeftBank = 26,
        Up25ToRightBank = 27,
        LeftBankToDown25 = 28,
        RightBankToDown25 = 29,
        Down25ToLeftBank = 30,
        Down25ToRightBank = 31,
        LeftBank = 32,
        RightBank = 33,
        LeftQuarterTurn5TilesUp25 = 34,
        RightQuarterTurn5TilesUp25 = 35,
        LeftQuarterTurn5TilesDown25 = 36,
        RightQuarterTurn5TilesDown25 = 37,
        SBendLeft = 38,
        SBendRight = 39,
        LeftVerticalLoop = 40,
        RightVerticalLoop = 41,
        LeftQuarterTurn3Tiles = 42,
        RightQuarterTurn3Tiles = 43,
        LeftBankedQuarterTurn3Tiles = 44,
        RightBankedQuarterTurn3Tiles = 45,
        LeftQuarterTurn3TilesUp25 = 46,
        RightQuarterTurn3TilesUp25 = 47,
        LeftQuarterTurn3TilesDown25 = 48,
        RightQuarterTurn3TilesDown25 = 49,
        LeftQuarterTurn1Tile = 50,
        RightQuarterTurn1Tile = 51,
        LeftTwistDownToUp = 52,
        RightTwistDownToUp = 53,
        LeftTwistUpToDown = 54,
        RightTwistUpToDown = 55,
        HalfLoopUp = 56,
        HalfLoopDown = 57,
        LeftCorkscrewUp = 58,
        RightCorkscrewUp = 59,
        LeftCorkscrewDown = 60,
        RightCorkscrewDown = 61,
        FlatToUp60 = 62,
        Up60ToFlat = 63,
        FlatToDown60 = 64,
        Down60ToFlat = 65,
        TowerBase = 66,
        TowerSection = 67,

        LeftHalfBankedHelixUpSmall = 68,
        RightHalfBankedHelixUpSmall = 69,
        LeftHalfBankedHelixDownSmall = 70,
        RightHalfBankedHelixDownSmall = 71,
        LeftHalfBankedHelixUpLarge = 72,
        RightHalfBankedHelixUpLarge = 73,
        LeftHalfBankedHelixDownLarge = 74,
        RightHalfBankedHelixDownLarge = 75,
        LeftQuarterTurn1TileUp60 = 76,
        RightQuarterTurn1TileUp60 = 77,
        LeftQuarterTurn1TileDown60 = 78,
        RightQuarterTurn1TileDown60 = 79,
        Brakes = 80,
        Booster = 81,
        Maze = 82,
        LeftQuarterBankedHelixLargeUp = 83,
        RightQuarterBankedHelixLargeUp = 84,
        LeftQuarterBankedHelixLargeDown = 85,
        RightQuarterBankedHelixLargeDown = 86,
        LeftQuarterHelixLargeUp = 87,
        RightQuarterHelixLargeUp = 88,
        LeftQuarterHelixLargeDown = 89,
        RightQuarterHelixLargeDown = 90,
        Up25LeftBanked = 91,
        Up25RightBanked = 92,
        Waterfall = 93,
        Rapids = 94,
        OnRidePhoto = 95,
        Down25LeftBanked = 96,
        Down25RightBanked = 97,
        Watersplash = 98,
        FlatToUp60LongBase = 99,
        Up60ToFlatLongBase = 100,
        Whirlpool = 101,
        Down60ToFlatLongBase = 102,
        FlatToDown60LongBase = 103,
        CableLiftHill = 104,
        ReverseFreefallSlope = 105,
        ReverseFreefallVertical = 106,
        Up90 = 107,
        Down90 = 108,
        Up60ToUp90 = 109,
        Down90ToDown60 = 110,
        Up90ToUp60 = 111,
        Down60ToDown90 = 112,
        BrakeForDrop = 113,
        LeftEighthToDiag = 114,
        RightEighthToDiag = 115,
        LeftEighthToOrthogonal = 116,
        RightEighthToOrthogonal = 117,
        LeftEighthBankToDiag = 118,
        RightEighthBankToDiag = 119,
        LeftEighthBankToOrthogonal = 120,
        RightEighthBankToOrthogonal = 121,
        DiagFlat = 122,
        DiagUp25 = 123,
        DiagUp60 = 124,
        DiagFlatToUp25 = 125,
        DiagUp25ToUp60 = 126,
        DiagUp60ToUp25 = 127,
        DiagUp25ToFlat = 128,
        DiagDown25 = 129,
        DiagDown60 = 130,
        DiagFlatToDown25 = 131,
        DiagDown25ToDown60 = 132,
        DiagDown60ToDown25 = 133,
        DiagDown25ToFlat = 134,
        DiagFlatToUp60 = 135,
        DiagUp60ToFlat = 136,
        DiagFlatToDown60 = 137,
        DiagDown60ToFlat = 138,
        DiagFlatToLeftBank = 139,
        DiagFlatToRightBank = 140,
        DiagLeftBankToFlat = 141,
        DiagRightBankToFlat = 142,
        DiagLeftBankToUp25 = 143,
        DiagRightBankToUp25 = 144,
        DiagUp25ToLeftBank = 145,
        DiagUp25ToRightBank = 146,
        DiagLeftBankToDown25 = 147,
        DiagRightBankToDown25 = 148,
        DiagDown25ToLeftBank = 149,
        DiagDown25ToRightBank = 150,
        DiagLeftBank = 151,
        DiagRightBank = 152,
        LogFlumeReverser = 153,
        SpinningTunnel = 154,
        LeftBarrelRollUpToDown = 155,
        RightBarrelRollUpToDown = 156,
        LeftBarrelRollDownToUp = 157,
        RightBarrelRollDownToUp = 158,
        LeftBankToLeftQuarterTurn3TilesUp25 = 159,
        RightBankToRightQuarterTurn3TilesUp25 = 160,
        LeftQuarterTurn3TilesDown25ToLeftBank = 161,
        RightQuarterTurn3TilesDown25ToRightBank = 162,
        PoweredLift = 163,
        LeftLargeHalfLoopUp = 164,
        RightLargeHalfLoopUp = 165,
        LeftLargeHalfLoopDown = 166,
        RightLargeHalfLoopDown = 167,
        LeftFlyerTwistUp = 168,
        RightFlyerTwistUp = 169,
        LeftFlyerTwistDown = 170,
        RightFlyerTwistDown = 171,
        FlyerHalfLoopUninvertedUp = 172,
        FlyerHalfLoopInvertedDown = 173,
        LeftFlyerCorkscrewUp = 174,
        RightFlyerCorkscrewUp = 175,
        LeftFlyerCorkscrewDown = 176,
        RightFlyerCorkscrewDown = 177,
        HeartLineTransferUp = 178,
        HeartLineTransferDown = 179,
        LeftHeartLineRoll = 180,
        RightHeartLineRoll = 181,
        MinigolfHoleA = 182,
        MinigolfHoleB = 183,
        MinigolfHoleC = 184,
        MinigolfHoleD = 185,
        MinigolfHoleE = 186,
        MultiDimInvertedFlatToDown90QuarterLoop = 187,
        Up90ToInvertedFlatQuarterLoop = 188,
        InvertedFlatToDown90QuarterLoop = 189,
        LeftCurvedLiftHill = 190,
        RightCurvedLiftHill = 191,
        LeftReverser = 192,
        RightReverser = 193,
        AirThrustTopCap = 194,
        AirThrustVerticalDown = 195,
        AirThrustVerticalDownToLevel = 196,
        BlockBrakes = 197,
        LeftBankedQuarterTurn3TileUp25 = 198,
        RightBankedQuarterTurn3TileUp25 = 199,
        LeftBankedQuarterTurn3TileDown25 = 200,
        RightBankedQuarterTurn3TileDown25 = 201,
        LeftBankedQuarterTurn5TileUp25 = 202,
        RightBankedQuarterTurn5TileUp25 = 203,
        LeftBankedQuarterTurn5TileDown25 = 204,
        RightBankedQuarterTurn5TileDown25 = 205,
        Up25ToLeftBankedUp25 = 206,
        Up25ToRightBankedUp25 = 207,
        LeftBankedUp25ToUp25 = 208,
        RightBankedUp25ToUp25 = 209,
        Down25ToLeftBankedDown25 = 210,
        Down25ToRightBankedDown25 = 211,
        LeftBankedDown25ToDown25 = 212,
        RightBankedDown25ToDown25 = 213,
        LeftBankedFlatToLeftBankedUp25 = 214,
        RightBankedFlatToRightBankedUp25 = 215,
        LeftBankedUp25ToLeftBankedFlat = 216,
        RightBankedUp25ToRightBankedFlat = 217,
        LeftBankedFlatToLeftBankedDown25 = 218,
        RightBankedFlatToRightBankedDown25 = 219,
        LeftBankedDown25ToLeftBankedFlat = 220,
        RightBankedDown25ToRightBankedFlat = 221,
        FlatToLeftBankedUp25 = 222,
        FlatToRightBankedUp25 = 223,
        LeftBankedUp25ToFlat = 224,
        RightBankedUp25ToFlat = 225,
        FlatToLeftBankedDown25 = 226,
        FlatToRightBankedDown25 = 227,
        LeftBankedDown25ToFlat = 228,
        RightBankedDown25ToFlat = 229,
        LeftQuarterTurn1TileUp90 = 230,
        RightQuarterTurn1TileUp90 = 231,
        LeftQuarterTurn1TileDown90 = 232,
        RightQuarterTurn1TileDown90 = 233,
        MultiDimUp90ToInvertedFlatQuarterLoop = 234,
        MultiDimFlatToDown90QuarterLoop = 235,
        MultiDimInvertedUp90ToFlatQuarterLoop = 236,

        // Elements that shared an ID with another piece in RCT1/2.
        RotationControlToggle = 237,
        FlatTrack1x4A = 238,
        FlatTrack2x2 = 239,
        FlatTrack4x4 = 240,
        FlatTrack2x4 = 241,
        FlatTrack1x5 = 242,
        FlatTrack1x1A = 243,
        FlatTrack1x4B = 244,
        FlatTrack1x1B = 245,
        FlatTrack1x4C = 246,
        FlatTrack3x3 = 247,

        // Highest track element ID that has a TD6 alias
        HighestAlias = 247,

        // Track Elements specific to OpenRCT2
        LeftLargeCorkscrewUp = 248,
        RightLargeCorkscrewUp = 249,
        LeftLargeCorkscrewDown = 250,
        RightLargeCorkscrewDown = 251,
        LeftMediumHalfLoopUp = 252,
        RightMediumHalfLoopUp = 253,
        LeftMediumHalfLoopDown = 254,
        RightMediumHalfLoopDown = 255,
        LeftZeroGRollUp = 256,
        RightZeroGRollUp = 257,
        LeftZeroGRollDown = 258,
        RightZeroGRollDown = 259,
        LeftLargeZeroGRollUp = 260,
        RightLargeZeroGRollUp = 261,
        LeftLargeZeroGRollDown = 262,
        RightLargeZeroGRollDown = 263,

        LeftFlyerLargeHalfLoopUninvertedUp = 264,
        RightFlyerLargeHalfLoopUninvertedUp = 265,
        LeftFlyerLargeHalfLoopInvertedDown = 266,
        RightFlyerLargeHalfLoopInvertedDown = 267,
        LeftFlyerLargeHalfLoopInvertedUp = 268,
        RightFlyerLargeHalfLoopInvertedUp = 269,
        LeftFlyerLargeHalfLoopUninvertedDown = 270,
        RightFlyerLargeHalfLoopUninvertedDown = 271,

        FlyerHalfLoopInvertedUp = 272,
        FlyerHalfLoopUninvertedDown = 273,

        LeftEighthToDiagUp25 = 274,
        RightEighthToDiagUp25 = 275,
        LeftEighthToDiagDown25 = 276,
        RightEighthToDiagDown25 = 277,
        LeftEighthToOrthogonalUp25 = 278,
        RightEighthToOrthogonalUp25 = 279,
        LeftEighthToOrthogonalDown25 = 280,
        RightEighthToOrthogonalDown25 = 281,

        DiagUp25ToLeftBankedUp25 = 282,
        DiagUp25ToRightBankedUp25 = 283,
        DiagLeftBankedUp25ToUp25 = 284,
        DiagRightBankedUp25ToUp25 = 285,
        DiagDown25ToLeftBankedDown25 = 286,
        DiagDown25ToRightBankedDown25 = 287,
        DiagLeftBankedDown25ToDown25 = 288,
        DiagRightBankedDown25ToDown25 = 289,
        DiagLeftBankedFlatToLeftBankedUp25 = 290,
        DiagRightBankedFlatToRightBankedUp25 = 291,
        DiagLeftBankedUp25ToLeftBankedFlat = 292,
        DiagRightBankedUp25ToRightBankedFlat = 293,
        DiagLeftBankedFlatToLeftBankedDown25 = 294,
        DiagRightBankedFlatToRightBankedDown25 = 295,
        DiagLeftBankedDown25ToLeftBankedFlat = 296,
        DiagRightBankedDown25ToRightBankedFlat = 297,
        DiagFlatToLeftBankedUp25 = 298,
        DiagFlatToRightBankedUp25 = 299,
        DiagLeftBankedUp25ToFlat = 300,
        DiagRightBankedUp25ToFlat = 301,
        DiagFlatToLeftBankedDown25 = 302,
        DiagFlatToRightBankedDown25 = 303,
        DiagLeftBankedDown25ToFlat = 304,
        DiagRightBankedDown25ToFlat = 305,
        DiagUp25LeftBanked = 306,
        DiagUp25RightBanked = 307,
        DiagDown25LeftBanked = 308,
        DiagDown25RightBanked = 309,

        LeftEighthBankToDiagUp25 = 310,
        RightEighthBankToDiagUp25 = 311,
        LeftEighthBankToDiagDown25 = 312,
        RightEighthBankToDiagDown25 = 313,
        LeftEighthBankToOrthogonalUp25 = 314,
        RightEighthBankToOrthogonalUp25 = 315,
        LeftEighthBankToOrthogonalDown25 = 316,
        RightEighthBankToOrthogonalDown25 = 317,

        DiagBrakes = 318,
        DiagBlockBrakes = 319,
        Down25Brakes = 320,
        DiagBooster = 321,

        DiagFlatToUp60LongBase = 322,
        DiagUp60ToFlatLongBase = 323,
        DiagFlatToDown60LongBase = 324,
        DiagDown60ToFlatLongBase = 325,
        LeftEighthDiveLoopUpToOrthogonal = 326,
        RightEighthDiveLoopUpToOrthogonal = 327,
        LeftEighthDiveLoopDownToDiag = 328,
        RightEighthDiveLoopDownToDiag = 329,

        Count = 330,

        None = 65535,
    };
}

enum
{
    TRACK_SEQUENCE_FLAG_DIRECTION_0 = (1 << 0),      // Ride Entrances and path connect to front
    TRACK_SEQUENCE_FLAG_DIRECTION_1 = (1 << 1),      // connect to right
    TRACK_SEQUENCE_FLAG_DIRECTION_2 = (1 << 2),      // connect to back
    TRACK_SEQUENCE_FLAG_DIRECTION_3 = (1 << 3),      // connect to left
    TRACK_SEQUENCE_FLAG_ORIGIN = (1 << 4),           // 0x10
    TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH = (1 << 5), // 0x20
    TRACK_SEQUENCE_FLAG_DISALLOW_DOORS = (1 << 6),   // 0x40
};

enum
{
    GC_SET_MAZE_TRACK_BUILD = 0,
    GC_SET_MAZE_TRACK_MOVE = 1,
    GC_SET_MAZE_TRACK_FILL = 2,
};

struct TrackCircuitIterator
{
    CoordsXYE last;
    CoordsXYE current;
    int32_t currentZ;
    int32_t currentDirection;
    TileElement* first;
    bool firstIteration;
    bool looped;
};

struct TypeOrCurve
{
    bool isTrackType = true; // true if a OpenRCT2::TrackElemType is selected, false if a TrackCurve is selected;
    union
    {
        OpenRCT2::TrackElemType trackType = OpenRCT2::TrackElemType::None;
        TrackCurve curve;
    };

    constexpr TypeOrCurve() noexcept
    {
        isTrackType = false;
        curve = TrackCurve::None;
    }

    constexpr bool operator==(OpenRCT2::TrackElemType rhs)
    {
        return isTrackType && (trackType == rhs);
    }

    constexpr bool operator==(TrackCurve rhs)
    {
        return !isTrackType && (curve == rhs);
    }

    constexpr TypeOrCurve(OpenRCT2::TrackElemType _type) noexcept
    {
        isTrackType = true;
        trackType = _type;
    }

    constexpr TypeOrCurve& operator=(OpenRCT2::TrackElemType rhs) noexcept
    {
        isTrackType = true;
        trackType = rhs;
        return *this;
    }

    constexpr TypeOrCurve(TrackCurve _curve) noexcept
    {
        isTrackType = false;
        curve = _curve;
    }

    constexpr TypeOrCurve& operator=(TrackCurve rhs) noexcept
    {
        isTrackType = false;
        curve = rhs;
        return *this;
    }
};

PitchAndRoll TrackPitchAndRollStart(OpenRCT2::TrackElemType trackType);
PitchAndRoll TrackPitchAndRollEnd(OpenRCT2::TrackElemType trackType);

int32_t TrackIsConnectedByShape(TileElement* a, TileElement* b);

void TrackCircuitIteratorBegin(TrackCircuitIterator* it, CoordsXYE first);
bool TrackCircuitIteratorPrevious(TrackCircuitIterator* it);
bool TrackCircuitIteratorNext(TrackCircuitIterator* it);
bool TrackCircuitIteratorsMatch(const TrackCircuitIterator* firstIt, const TrackCircuitIterator* secondIt);

void TrackGetBack(const CoordsXYE& input, CoordsXYE* output);
void TrackGetFront(const CoordsXYE& input, CoordsXYE* output);

TrackElement* TrackGetPreviousBlock(CoordsXYZ& location, TileElement* tileElement);

bool TrackTypeIsStation(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsBrakes(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsBlockBrakes(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsBooster(OpenRCT2::TrackElemType trackType);

std::optional<CoordsXYZ> GetTrackElementOriginAndApplyChanges(
    const CoordsXYZD& location, OpenRCT2::TrackElemType type, bool isCovered, uint16_t extra_params,
    TileElement** output_element, uint16_t flags);

TrackRoll TrackGetActualBank(TileElement* tileElement, TrackRoll bank);
TrackRoll TrackGetActualBank2(ride_type_t rideType, bool isInverted, TrackRoll bank);
TrackRoll TrackGetActualBank3(bool useInvertedSprites, TileElement* tileElement);

ResultWithMessage TrackAddStationElement(CoordsXYZD loc, RideId rideIndex, int32_t flags, bool fromTrackDesign);
ResultWithMessage TrackRemoveStationElement(const CoordsXYZD& loc, RideId rideIndex, int32_t flags);

bool TrackTypeHasSpeedSetting(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsHelix(OpenRCT2::TrackElemType trackType);
std::optional<CoordsXYZD> GetTrackSegmentOrigin(const CoordsXYE& posEl);
