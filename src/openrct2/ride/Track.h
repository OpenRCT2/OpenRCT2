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
constexpr auto kTrackDirectionDiagonalMask = 0b0100;

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
        FlatCovered = 68,
        Up25Covered = 69,
        Up60Covered = 70,
        FlatToUp25Covered = 71,
        Up25ToUp60Covered = 72,
        Up60ToUp25Covered = 73,
        Up25ToFlatCovered = 74,
        Down25Covered = 75,
        Down60Covered = 76,
        FlatToDown25Covered = 77,
        Down25ToDown60Covered = 78,
        Down60ToDown25Covered = 79,
        Down25ToFlatCovered = 80,
        LeftQuarterTurn5TilesCovered = 81,
        RightQuarterTurn5TilesCovered = 82,
        SBendLeftCovered = 83,
        SBendRightCovered = 84,
        LeftQuarterTurn3TilesCovered = 85,
        RightQuarterTurn3TilesCovered = 86,
        LeftHalfBankedHelixUpSmall = 87,
        RightHalfBankedHelixUpSmall = 88,
        LeftHalfBankedHelixDownSmall = 89,
        RightHalfBankedHelixDownSmall = 90,
        LeftHalfBankedHelixUpLarge = 91,
        RightHalfBankedHelixUpLarge = 92,
        LeftHalfBankedHelixDownLarge = 93,
        RightHalfBankedHelixDownLarge = 94,
        LeftQuarterTurn1TileUp60 = 95,
        RightQuarterTurn1TileUp60 = 96,
        LeftQuarterTurn1TileDown60 = 97,
        RightQuarterTurn1TileDown60 = 98,
        Brakes = 99,
        Booster = 100,
        Maze = 101,
        LeftQuarterBankedHelixLargeUp = 102,
        RightQuarterBankedHelixLargeUp = 103,
        LeftQuarterBankedHelixLargeDown = 104,
        RightQuarterBankedHelixLargeDown = 105,
        LeftQuarterHelixLargeUp = 106,
        RightQuarterHelixLargeUp = 107,
        LeftQuarterHelixLargeDown = 108,
        RightQuarterHelixLargeDown = 109,
        Up25LeftBanked = 110,
        Up25RightBanked = 111,
        Waterfall = 112,
        Rapids = 113,
        OnRidePhoto = 114,
        Down25LeftBanked = 115,
        Down25RightBanked = 116,
        Watersplash = 117,
        FlatToUp60LongBase = 118,
        Up60ToFlatLongBase = 119,
        Whirlpool = 120,
        Down60ToFlatLongBase = 121,
        FlatToDown60LongBase = 122,
        CableLiftHill = 123,
        ReverseFreefallSlope = 124,
        ReverseFreefallVertical = 125,
        Up90 = 126,
        Down90 = 127,
        Up60ToUp90 = 128,
        Down90ToDown60 = 129,
        Up90ToUp60 = 130,
        Down60ToDown90 = 131,
        BrakeForDrop = 132,
        LeftEighthToDiag = 133,
        RightEighthToDiag = 134,
        LeftEighthToOrthogonal = 135,
        RightEighthToOrthogonal = 136,
        LeftEighthBankToDiag = 137,
        RightEighthBankToDiag = 138,
        LeftEighthBankToOrthogonal = 139,
        RightEighthBankToOrthogonal = 140,
        DiagFlat = 141,
        DiagUp25 = 142,
        DiagUp60 = 143,
        DiagFlatToUp25 = 144,
        DiagUp25ToUp60 = 145,
        DiagUp60ToUp25 = 146,
        DiagUp25ToFlat = 147,
        DiagDown25 = 148,
        DiagDown60 = 149,
        DiagFlatToDown25 = 150,
        DiagDown25ToDown60 = 151,
        DiagDown60ToDown25 = 152,
        DiagDown25ToFlat = 153,
        DiagFlatToUp60 = 154,
        DiagUp60ToFlat = 155,
        DiagFlatToDown60 = 156,
        DiagDown60ToFlat = 157,
        DiagFlatToLeftBank = 158,
        DiagFlatToRightBank = 159,
        DiagLeftBankToFlat = 160,
        DiagRightBankToFlat = 161,
        DiagLeftBankToUp25 = 162,
        DiagRightBankToUp25 = 163,
        DiagUp25ToLeftBank = 164,
        DiagUp25ToRightBank = 165,
        DiagLeftBankToDown25 = 166,
        DiagRightBankToDown25 = 167,
        DiagDown25ToLeftBank = 168,
        DiagDown25ToRightBank = 169,
        DiagLeftBank = 170,
        DiagRightBank = 171,
        LogFlumeReverser = 172,
        SpinningTunnel = 173,
        LeftBarrelRollUpToDown = 174,
        RightBarrelRollUpToDown = 175,
        LeftBarrelRollDownToUp = 176,
        RightBarrelRollDownToUp = 177,
        LeftBankToLeftQuarterTurn3TilesUp25 = 178,
        RightBankToRightQuarterTurn3TilesUp25 = 179,
        LeftQuarterTurn3TilesDown25ToLeftBank = 180,
        RightQuarterTurn3TilesDown25ToRightBank = 181,
        PoweredLift = 182,
        LeftLargeHalfLoopUp = 183,
        RightLargeHalfLoopUp = 184,
        LeftLargeHalfLoopDown = 185,
        RightLargeHalfLoopDown = 186,
        LeftFlyerTwistUp = 187,
        RightFlyerTwistUp = 188,
        LeftFlyerTwistDown = 189,
        RightFlyerTwistDown = 190,
        FlyerHalfLoopUninvertedUp = 191,
        FlyerHalfLoopInvertedDown = 192,
        LeftFlyerCorkscrewUp = 193,
        RightFlyerCorkscrewUp = 194,
        LeftFlyerCorkscrewDown = 195,
        RightFlyerCorkscrewDown = 196,
        HeartLineTransferUp = 197,
        HeartLineTransferDown = 198,
        LeftHeartLineRoll = 199,
        RightHeartLineRoll = 200,
        MinigolfHoleA = 201,
        MinigolfHoleB = 202,
        MinigolfHoleC = 203,
        MinigolfHoleD = 204,
        MinigolfHoleE = 205,
        MultiDimInvertedFlatToDown90QuarterLoop = 206,
        Up90ToInvertedFlatQuarterLoop = 207,
        InvertedFlatToDown90QuarterLoop = 208,
        LeftCurvedLiftHill = 209,
        RightCurvedLiftHill = 210,
        LeftReverser = 211,
        RightReverser = 212,
        AirThrustTopCap = 213,
        AirThrustVerticalDown = 214,
        AirThrustVerticalDownToLevel = 215,
        BlockBrakes = 216,
        LeftBankedQuarterTurn3TileUp25 = 217,
        RightBankedQuarterTurn3TileUp25 = 218,
        LeftBankedQuarterTurn3TileDown25 = 219,
        RightBankedQuarterTurn3TileDown25 = 220,
        LeftBankedQuarterTurn5TileUp25 = 221,
        RightBankedQuarterTurn5TileUp25 = 222,
        LeftBankedQuarterTurn5TileDown25 = 223,
        RightBankedQuarterTurn5TileDown25 = 224,
        Up25ToLeftBankedUp25 = 225,
        Up25ToRightBankedUp25 = 226,
        LeftBankedUp25ToUp25 = 227,
        RightBankedUp25ToUp25 = 228,
        Down25ToLeftBankedDown25 = 229,
        Down25ToRightBankedDown25 = 230,
        LeftBankedDown25ToDown25 = 231,
        RightBankedDown25ToDown25 = 232,
        LeftBankedFlatToLeftBankedUp25 = 233,
        RightBankedFlatToRightBankedUp25 = 234,
        LeftBankedUp25ToLeftBankedFlat = 235,
        RightBankedUp25ToRightBankedFlat = 236,
        LeftBankedFlatToLeftBankedDown25 = 237,
        RightBankedFlatToRightBankedDown25 = 238,
        LeftBankedDown25ToLeftBankedFlat = 239,
        RightBankedDown25ToRightBankedFlat = 240,
        FlatToLeftBankedUp25 = 241,
        FlatToRightBankedUp25 = 242,
        LeftBankedUp25ToFlat = 243,
        RightBankedUp25ToFlat = 244,
        FlatToLeftBankedDown25 = 245,
        FlatToRightBankedDown25 = 246,
        LeftBankedDown25ToFlat = 247,
        RightBankedDown25ToFlat = 248,
        LeftQuarterTurn1TileUp90 = 249,
        RightQuarterTurn1TileUp90 = 250,
        LeftQuarterTurn1TileDown90 = 251,
        RightQuarterTurn1TileDown90 = 252,
        MultiDimUp90ToInvertedFlatQuarterLoop = 253,
        MultiDimFlatToDown90QuarterLoop = 254,
        MultiDimInvertedUp90ToFlatQuarterLoop = 255,

        // Elements that shared an ID with another piece in RCT1/2.
        RotationControlToggle = 256,
        FlatTrack1x4A = 257,
        FlatTrack2x2 = 258,
        FlatTrack4x4 = 259,
        FlatTrack2x4 = 260,
        FlatTrack1x5 = 261,
        FlatTrack1x1A = 262,
        FlatTrack1x4B = 263,
        FlatTrack1x1B = 264,
        FlatTrack1x4C = 265,
        FlatTrack3x3 = 266,

        // Highest track element ID that has a TD6 alias
        HighestAlias = 266,

        // Track Elements specific to OpenRCT2
        LeftLargeCorkscrewUp = 267,
        RightLargeCorkscrewUp = 268,
        LeftLargeCorkscrewDown = 269,
        RightLargeCorkscrewDown = 270,
        LeftMediumHalfLoopUp = 271,
        RightMediumHalfLoopUp = 272,
        LeftMediumHalfLoopDown = 273,
        RightMediumHalfLoopDown = 274,
        LeftZeroGRollUp = 275,
        RightZeroGRollUp = 276,
        LeftZeroGRollDown = 277,
        RightZeroGRollDown = 278,
        LeftLargeZeroGRollUp = 279,
        RightLargeZeroGRollUp = 280,
        LeftLargeZeroGRollDown = 281,
        RightLargeZeroGRollDown = 282,

        LeftFlyerLargeHalfLoopUninvertedUp = 283,
        RightFlyerLargeHalfLoopUninvertedUp = 284,
        LeftFlyerLargeHalfLoopInvertedDown = 285,
        RightFlyerLargeHalfLoopInvertedDown = 286,
        LeftFlyerLargeHalfLoopInvertedUp = 287,
        RightFlyerLargeHalfLoopInvertedUp = 288,
        LeftFlyerLargeHalfLoopUninvertedDown = 289,
        RightFlyerLargeHalfLoopUninvertedDown = 290,

        FlyerHalfLoopInvertedUp = 291,
        FlyerHalfLoopUninvertedDown = 292,

        LeftEighthToDiagUp25 = 293,
        RightEighthToDiagUp25 = 294,
        LeftEighthToDiagDown25 = 295,
        RightEighthToDiagDown25 = 296,
        LeftEighthToOrthogonalUp25 = 297,
        RightEighthToOrthogonalUp25 = 298,
        LeftEighthToOrthogonalDown25 = 299,
        RightEighthToOrthogonalDown25 = 300,

        DiagUp25ToLeftBankedUp25 = 301,
        DiagUp25ToRightBankedUp25 = 302,
        DiagLeftBankedUp25ToUp25 = 303,
        DiagRightBankedUp25ToUp25 = 304,
        DiagDown25ToLeftBankedDown25 = 305,
        DiagDown25ToRightBankedDown25 = 306,
        DiagLeftBankedDown25ToDown25 = 307,
        DiagRightBankedDown25ToDown25 = 308,
        DiagLeftBankedFlatToLeftBankedUp25 = 309,
        DiagRightBankedFlatToRightBankedUp25 = 310,
        DiagLeftBankedUp25ToLeftBankedFlat = 311,
        DiagRightBankedUp25ToRightBankedFlat = 312,
        DiagLeftBankedFlatToLeftBankedDown25 = 313,
        DiagRightBankedFlatToRightBankedDown25 = 314,
        DiagLeftBankedDown25ToLeftBankedFlat = 315,
        DiagRightBankedDown25ToRightBankedFlat = 316,
        DiagFlatToLeftBankedUp25 = 317,
        DiagFlatToRightBankedUp25 = 318,
        DiagLeftBankedUp25ToFlat = 319,
        DiagRightBankedUp25ToFlat = 320,
        DiagFlatToLeftBankedDown25 = 321,
        DiagFlatToRightBankedDown25 = 322,
        DiagLeftBankedDown25ToFlat = 323,
        DiagRightBankedDown25ToFlat = 324,
        DiagUp25LeftBanked = 325,
        DiagUp25RightBanked = 326,
        DiagDown25LeftBanked = 327,
        DiagDown25RightBanked = 328,

        LeftEighthBankToDiagUp25 = 329,
        RightEighthBankToDiagUp25 = 330,
        LeftEighthBankToDiagDown25 = 331,
        RightEighthBankToDiagDown25 = 332,
        LeftEighthBankToOrthogonalUp25 = 333,
        RightEighthBankToOrthogonalUp25 = 334,
        LeftEighthBankToOrthogonalDown25 = 335,
        RightEighthBankToOrthogonalDown25 = 336,

        DiagBrakes = 337,
        DiagBlockBrakes = 338,
        Down25Brakes = 339,
        DiagBooster = 340,

        DiagFlatToUp60LongBase = 341,
        DiagUp60ToFlatLongBase = 342,
        DiagFlatToDown60LongBase = 343,
        DiagDown60ToFlatLongBase = 344,
        LeftEighthDiveLoopUpToOrthogonal = 345,
        RightEighthDiveLoopUpToOrthogonal = 346,
        LeftEighthDiveLoopDownToDiag = 347,
        RightEighthDiveLoopDownToDiag = 348,

        Count = 349,

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

bool TrackElementIsCovered(OpenRCT2::TrackElemType trackElementType);
OpenRCT2::TrackElemType UncoverTrackElement(OpenRCT2::TrackElemType trackElementType);
bool TrackTypeIsStation(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsBrakes(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsBlockBrakes(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsBooster(OpenRCT2::TrackElemType trackType);

std::optional<CoordsXYZ> GetTrackElementOriginAndApplyChanges(
    const CoordsXYZD& location, OpenRCT2::TrackElemType type, uint16_t extra_params, TileElement** output_element,
    uint16_t flags);

TrackRoll TrackGetActualBank(TileElement* tileElement, TrackRoll bank);
TrackRoll TrackGetActualBank2(ride_type_t rideType, bool isInverted, TrackRoll bank);
TrackRoll TrackGetActualBank3(bool useInvertedSprites, TileElement* tileElement);

ResultWithMessage TrackAddStationElement(CoordsXYZD loc, RideId rideIndex, int32_t flags, bool fromTrackDesign);
ResultWithMessage TrackRemoveStationElement(const CoordsXYZD& loc, RideId rideIndex, int32_t flags);

bool TrackTypeHasSpeedSetting(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsHelix(OpenRCT2::TrackElemType trackType);
std::optional<CoordsXYZD> GetTrackSegmentOrigin(const CoordsXYE& posEl);

constexpr bool TrackPieceDirectionIsDiagonal(const uint8_t direction)
{
    return direction & kTrackDirectionDiagonalMask;
}
