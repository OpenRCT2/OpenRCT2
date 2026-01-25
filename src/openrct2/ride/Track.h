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
#include "../actions/CommandFlag.h"
#include "../world/Location.hpp"
#include "../world/QuarterTile.h"
#include "RideTypes.h"

#include <optional>

constexpr uint8_t kRCT2DefaultBlockBrakeSpeed = 2;
constexpr int32_t kBlockBrakeBaseSpeed = 0x20364;
constexpr int32_t kBlockBrakeSpeedOffset = kBlockBrakeBaseSpeed - (kRCT2DefaultBlockBrakeSpeed << 16);
constexpr auto kTrackDirectionDiagonalMask = 0b0100;

constexpr uint8_t kMaximumTrackSpeed = 30;

constexpr int8_t kMaxStationPlatformLength = 32;
constexpr uint16_t kMaximumTrackHeight = 254 * kCoordsZStep;
constexpr uint8_t kDefaultSeatRotation = 4;

namespace OpenRCT2
{
    enum class TrackElemType : uint16_t;

    struct TileElement;
    struct TrackElement;
} // namespace OpenRCT2

struct ResultWithMessage;

enum class TrackRoll : uint8_t
{
    none = 0,
    left = 2,
    right = 4,
    upsideDown = 15,
};

enum class TrackPitch : uint8_t
{
    none = 0,
    up25 = 2,
    up60 = 4,
    down25 = 6,
    down60 = 8,
    up90 = 10,
    down90 = 18,

    tower = 10,
    reverseFreefall = 10
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

    diagSlope,
    diagSlopeSteepUp,
    diagSlopeSteepDown,

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

/** TrackCoordinates
 * rotationBegin: Start direction of the track piece; 0 (cardinal) or 4 (45 degrees clockwise)
 * rotationEnd: End direction of the track peice, measured clockwise
 * zBegin: how high above the base level of the lowest track block the spline starts
 * zEnd: how high above the base level of the lowest track block the spline ends
 * x: tile offset to the end track block; negative is along the original direction of travel
 * Next track piece is found by moving one tile after end of the track piece, so vertical track has an x value of 32
 * y: tile offset to the end track block; negative is to the left of the original direction of travel
 */
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

enum class TrackCurve : uint8_t
{
    leftVerySmall = 5,
    leftSmall = 3,
    left = 1,
    leftLarge = 7,
    none = 0,
    rightLarge = 8,
    right = 2,
    rightSmall = 4,
    rightVerySmall = 6
};
constexpr const uint8_t kHighestCurveValue = 8;

enum class TrackElementFlag : uint8_t
{
    onlyUnderwater,
    turnLeft,
    turnRight,
    turnBanked,
    turnSloped,
    down,
    up,
    normalToInversion,
    isGolfHole = normalToInversion,
    startsAtHalfHeight,
    onlyAboveGround,

    // Used to allow steep backwards lifts on roller coasters that do not allow steep
    // forward lift hills
    isSteepUp,
    helix,
    allowLiftHill,
    curveAllowsLift,
    inversionToNormal,

    // Also set on Spinning Tunnel and Log Flume reverser, probably to save a flag.
    banked,
    canBePartlyUnderground,
};

namespace OpenRCT2
{
    enum class TrackElemType : uint16_t
    {
        flat = 0,
        endStation = 1,
        beginStation = 2,
        middleStation = 3,
        up25 = 4,
        up60 = 5,
        flatToUp25 = 6,
        up25ToUp60 = 7,
        up60ToUp25 = 8,
        up25ToFlat = 9,
        down25 = 10,
        down60 = 11,
        flatToDown25 = 12,
        down25ToDown60 = 13,
        down60ToDown25 = 14,
        down25ToFlat = 15,
        leftQuarterTurn5Tiles = 16,
        rightQuarterTurn5Tiles = 17,
        flatToLeftBank = 18,
        flatToRightBank = 19,
        leftBankToFlat = 20,
        rightBankToFlat = 21,
        bankedLeftQuarterTurn5Tiles = 22,
        bankedRightQuarterTurn5Tiles = 23,
        leftBankToUp25 = 24,
        rightBankToUp25 = 25,
        up25ToLeftBank = 26,
        up25ToRightBank = 27,
        leftBankToDown25 = 28,
        rightBankToDown25 = 29,
        down25ToLeftBank = 30,
        down25ToRightBank = 31,
        leftBank = 32,
        rightBank = 33,
        leftQuarterTurn5TilesUp25 = 34,
        rightQuarterTurn5TilesUp25 = 35,
        leftQuarterTurn5TilesDown25 = 36,
        rightQuarterTurn5TilesDown25 = 37,
        sBendLeft = 38,
        sBendRight = 39,
        leftVerticalLoop = 40,
        rightVerticalLoop = 41,
        leftQuarterTurn3Tiles = 42,
        rightQuarterTurn3Tiles = 43,
        leftBankedQuarterTurn3Tiles = 44,
        rightBankedQuarterTurn3Tiles = 45,
        leftQuarterTurn3TilesUp25 = 46,
        rightQuarterTurn3TilesUp25 = 47,
        leftQuarterTurn3TilesDown25 = 48,
        rightQuarterTurn3TilesDown25 = 49,
        leftQuarterTurn1Tile = 50,
        rightQuarterTurn1Tile = 51,
        leftTwistDownToUp = 52,
        rightTwistDownToUp = 53,
        leftTwistUpToDown = 54,
        rightTwistUpToDown = 55,
        halfLoopUp = 56,
        halfLoopDown = 57,
        leftCorkscrewUp = 58,
        rightCorkscrewUp = 59,
        leftCorkscrewDown = 60,
        rightCorkscrewDown = 61,
        flatToUp60 = 62,
        up60ToFlat = 63,
        flatToDown60 = 64,
        down60ToFlat = 65,
        towerBase = 66,
        towerSection = 67,
        flatCovered = 68,
        up25Covered = 69,
        up60Covered = 70,
        flatToUp25Covered = 71,
        up25ToUp60Covered = 72,
        up60ToUp25Covered = 73,
        up25ToFlatCovered = 74,
        down25Covered = 75,
        down60Covered = 76,
        flatToDown25Covered = 77,
        down25ToDown60Covered = 78,
        down60ToDown25Covered = 79,
        down25ToFlatCovered = 80,
        leftQuarterTurn5TilesCovered = 81,
        rightQuarterTurn5TilesCovered = 82,
        sBendLeftCovered = 83,
        sBendRightCovered = 84,
        leftQuarterTurn3TilesCovered = 85,
        rightQuarterTurn3TilesCovered = 86,
        leftHalfBankedHelixUpSmall = 87,
        rightHalfBankedHelixUpSmall = 88,
        leftHalfBankedHelixDownSmall = 89,
        rightHalfBankedHelixDownSmall = 90,
        leftHalfBankedHelixUpLarge = 91,
        rightHalfBankedHelixUpLarge = 92,
        leftHalfBankedHelixDownLarge = 93,
        rightHalfBankedHelixDownLarge = 94,
        leftQuarterTurn1TileUp60 = 95,
        rightQuarterTurn1TileUp60 = 96,
        leftQuarterTurn1TileDown60 = 97,
        rightQuarterTurn1TileDown60 = 98,
        brakes = 99,
        booster = 100,
        maze = 101,
        leftQuarterBankedHelixLargeUp = 102,
        rightQuarterBankedHelixLargeUp = 103,
        leftQuarterBankedHelixLargeDown = 104,
        rightQuarterBankedHelixLargeDown = 105,
        leftQuarterHelixLargeUp = 106,
        rightQuarterHelixLargeUp = 107,
        leftQuarterHelixLargeDown = 108,
        rightQuarterHelixLargeDown = 109,
        up25LeftBanked = 110,
        up25RightBanked = 111,
        waterfall = 112,
        rapids = 113,
        onRidePhoto = 114,
        down25LeftBanked = 115,
        down25RightBanked = 116,
        watersplash = 117,
        flatToUp60LongBase = 118,
        up60ToFlatLongBase = 119,
        whirlpool = 120,
        down60ToFlatLongBase = 121,
        flatToDown60LongBase = 122,
        cableLiftHill = 123,
        reverseFreefallSlope = 124,
        reverseFreefallVertical = 125,
        up90 = 126,
        down90 = 127,
        up60ToUp90 = 128,
        down90ToDown60 = 129,
        up90ToUp60 = 130,
        down60ToDown90 = 131,
        brakeForDrop = 132,
        leftEighthToDiag = 133,
        rightEighthToDiag = 134,
        leftEighthToOrthogonal = 135,
        rightEighthToOrthogonal = 136,
        leftEighthBankToDiag = 137,
        rightEighthBankToDiag = 138,
        leftEighthBankToOrthogonal = 139,
        rightEighthBankToOrthogonal = 140,
        diagFlat = 141,
        diagUp25 = 142,
        diagUp60 = 143,
        diagFlatToUp25 = 144,
        diagUp25ToUp60 = 145,
        diagUp60ToUp25 = 146,
        diagUp25ToFlat = 147,
        diagDown25 = 148,
        diagDown60 = 149,
        diagFlatToDown25 = 150,
        diagDown25ToDown60 = 151,
        diagDown60ToDown25 = 152,
        diagDown25ToFlat = 153,
        diagFlatToUp60 = 154,
        diagUp60ToFlat = 155,
        diagFlatToDown60 = 156,
        diagDown60ToFlat = 157,
        diagFlatToLeftBank = 158,
        diagFlatToRightBank = 159,
        diagLeftBankToFlat = 160,
        diagRightBankToFlat = 161,
        diagLeftBankToUp25 = 162,
        diagRightBankToUp25 = 163,
        diagUp25ToLeftBank = 164,
        diagUp25ToRightBank = 165,
        diagLeftBankToDown25 = 166,
        diagRightBankToDown25 = 167,
        diagDown25ToLeftBank = 168,
        diagDown25ToRightBank = 169,
        diagLeftBank = 170,
        diagRightBank = 171,
        logFlumeReverser = 172,
        spinningTunnel = 173,
        leftBarrelRollUpToDown = 174,
        rightBarrelRollUpToDown = 175,
        leftBarrelRollDownToUp = 176,
        rightBarrelRollDownToUp = 177,
        leftBankToLeftQuarterTurn3TilesUp25 = 178,
        rightBankToRightQuarterTurn3TilesUp25 = 179,
        leftQuarterTurn3TilesDown25ToLeftBank = 180,
        rightQuarterTurn3TilesDown25ToRightBank = 181,
        poweredLift = 182,
        leftLargeHalfLoopUp = 183,
        rightLargeHalfLoopUp = 184,
        leftLargeHalfLoopDown = 185,
        rightLargeHalfLoopDown = 186,
        leftFlyerTwistUp = 187,
        rightFlyerTwistUp = 188,
        leftFlyerTwistDown = 189,
        rightFlyerTwistDown = 190,
        flyerHalfLoopUninvertedUp = 191,
        flyerHalfLoopInvertedDown = 192,
        leftFlyerCorkscrewUp = 193,
        rightFlyerCorkscrewUp = 194,
        leftFlyerCorkscrewDown = 195,
        rightFlyerCorkscrewDown = 196,
        heartLineTransferUp = 197,
        heartLineTransferDown = 198,
        leftHeartLineRoll = 199,
        rightHeartLineRoll = 200,
        minigolfHoleA = 201,
        minigolfHoleB = 202,
        minigolfHoleC = 203,
        minigolfHoleD = 204,
        minigolfHoleE = 205,
        multiDimInvertedFlatToDown90QuarterLoop = 206,
        up90ToInvertedFlatQuarterLoop = 207,
        invertedFlatToDown90QuarterLoop = 208,
        leftCurvedLiftHill = 209,
        rightCurvedLiftHill = 210,
        leftReverser = 211,
        rightReverser = 212,
        airThrustTopCap = 213,
        airThrustVerticalDown = 214,
        airThrustVerticalDownToLevel = 215,
        blockBrakes = 216,
        leftBankedQuarterTurn3TileUp25 = 217,
        rightBankedQuarterTurn3TileUp25 = 218,
        leftBankedQuarterTurn3TileDown25 = 219,
        rightBankedQuarterTurn3TileDown25 = 220,
        leftBankedQuarterTurn5TileUp25 = 221,
        rightBankedQuarterTurn5TileUp25 = 222,
        leftBankedQuarterTurn5TileDown25 = 223,
        rightBankedQuarterTurn5TileDown25 = 224,
        up25ToLeftBankedUp25 = 225,
        up25ToRightBankedUp25 = 226,
        leftBankedUp25ToUp25 = 227,
        rightBankedUp25ToUp25 = 228,
        down25ToLeftBankedDown25 = 229,
        down25ToRightBankedDown25 = 230,
        leftBankedDown25ToDown25 = 231,
        rightBankedDown25ToDown25 = 232,
        leftBankedFlatToLeftBankedUp25 = 233,
        rightBankedFlatToRightBankedUp25 = 234,
        leftBankedUp25ToLeftBankedFlat = 235,
        rightBankedUp25ToRightBankedFlat = 236,
        leftBankedFlatToLeftBankedDown25 = 237,
        rightBankedFlatToRightBankedDown25 = 238,
        leftBankedDown25ToLeftBankedFlat = 239,
        rightBankedDown25ToRightBankedFlat = 240,
        flatToLeftBankedUp25 = 241,
        flatToRightBankedUp25 = 242,
        leftBankedUp25ToFlat = 243,
        rightBankedUp25ToFlat = 244,
        flatToLeftBankedDown25 = 245,
        flatToRightBankedDown25 = 246,
        leftBankedDown25ToFlat = 247,
        rightBankedDown25ToFlat = 248,
        leftQuarterTurn1TileUp90 = 249,
        rightQuarterTurn1TileUp90 = 250,
        leftQuarterTurn1TileDown90 = 251,
        rightQuarterTurn1TileDown90 = 252,
        multiDimUp90ToInvertedFlatQuarterLoop = 253,
        multiDimFlatToDown90QuarterLoop = 254,
        multiDimInvertedUp90ToFlatQuarterLoop = 255,

        // Elements that shared an ID with another piece in RCT1/2.
        rotationControlToggle = 256,
        flatTrack1x4A = 257,
        flatTrack2x2 = 258,
        flatTrack4x4 = 259,
        flatTrack2x4 = 260,
        flatTrack1x5 = 261,
        flatTrack1x1A = 262,
        flatTrack1x4B = 263,
        flatTrack1x1B = 264,
        flatTrack1x4C = 265,
        flatTrack3x3 = 266,

        // Highest track element ID that has a TD6 alias
        highestAlias = 266,

        // Track Elements specific to OpenRCT2
        leftLargeCorkscrewUp = 267,
        rightLargeCorkscrewUp = 268,
        leftLargeCorkscrewDown = 269,
        rightLargeCorkscrewDown = 270,
        leftMediumHalfLoopUp = 271,
        rightMediumHalfLoopUp = 272,
        leftMediumHalfLoopDown = 273,
        rightMediumHalfLoopDown = 274,
        leftZeroGRollUp = 275,
        rightZeroGRollUp = 276,
        leftZeroGRollDown = 277,
        rightZeroGRollDown = 278,
        leftLargeZeroGRollUp = 279,
        rightLargeZeroGRollUp = 280,
        leftLargeZeroGRollDown = 281,
        rightLargeZeroGRollDown = 282,

        leftFlyerLargeHalfLoopUninvertedUp = 283,
        rightFlyerLargeHalfLoopUninvertedUp = 284,
        leftFlyerLargeHalfLoopInvertedDown = 285,
        rightFlyerLargeHalfLoopInvertedDown = 286,
        leftFlyerLargeHalfLoopInvertedUp = 287,
        rightFlyerLargeHalfLoopInvertedUp = 288,
        leftFlyerLargeHalfLoopUninvertedDown = 289,
        rightFlyerLargeHalfLoopUninvertedDown = 290,

        flyerHalfLoopInvertedUp = 291,
        flyerHalfLoopUninvertedDown = 292,

        leftEighthToDiagUp25 = 293,
        rightEighthToDiagUp25 = 294,
        leftEighthToDiagDown25 = 295,
        rightEighthToDiagDown25 = 296,
        leftEighthToOrthogonalUp25 = 297,
        rightEighthToOrthogonalUp25 = 298,
        leftEighthToOrthogonalDown25 = 299,
        rightEighthToOrthogonalDown25 = 300,

        diagUp25ToLeftBankedUp25 = 301,
        diagUp25ToRightBankedUp25 = 302,
        diagLeftBankedUp25ToUp25 = 303,
        diagRightBankedUp25ToUp25 = 304,
        diagDown25ToLeftBankedDown25 = 305,
        diagDown25ToRightBankedDown25 = 306,
        diagLeftBankedDown25ToDown25 = 307,
        diagRightBankedDown25ToDown25 = 308,
        diagLeftBankedFlatToLeftBankedUp25 = 309,
        diagRightBankedFlatToRightBankedUp25 = 310,
        diagLeftBankedUp25ToLeftBankedFlat = 311,
        diagRightBankedUp25ToRightBankedFlat = 312,
        diagLeftBankedFlatToLeftBankedDown25 = 313,
        diagRightBankedFlatToRightBankedDown25 = 314,
        diagLeftBankedDown25ToLeftBankedFlat = 315,
        diagRightBankedDown25ToRightBankedFlat = 316,
        diagFlatToLeftBankedUp25 = 317,
        diagFlatToRightBankedUp25 = 318,
        diagLeftBankedUp25ToFlat = 319,
        diagRightBankedUp25ToFlat = 320,
        diagFlatToLeftBankedDown25 = 321,
        diagFlatToRightBankedDown25 = 322,
        diagLeftBankedDown25ToFlat = 323,
        diagRightBankedDown25ToFlat = 324,
        diagUp25LeftBanked = 325,
        diagUp25RightBanked = 326,
        diagDown25LeftBanked = 327,
        diagDown25RightBanked = 328,

        leftEighthBankToDiagUp25 = 329,
        rightEighthBankToDiagUp25 = 330,
        leftEighthBankToDiagDown25 = 331,
        rightEighthBankToDiagDown25 = 332,
        leftEighthBankToOrthogonalUp25 = 333,
        rightEighthBankToOrthogonalUp25 = 334,
        leftEighthBankToOrthogonalDown25 = 335,
        rightEighthBankToOrthogonalDown25 = 336,

        diagBrakes = 337,
        diagBlockBrakes = 338,
        down25Brakes = 339,
        diagBooster = 340,

        diagFlatToUp60LongBase = 341,
        diagUp60ToFlatLongBase = 342,
        diagFlatToDown60LongBase = 343,
        diagDown60ToFlatLongBase = 344,
        leftEighthDiveLoopUpToOrthogonal = 345,
        rightEighthDiveLoopUpToOrthogonal = 346,
        leftEighthDiveLoopDownToDiag = 347,
        rightEighthDiveLoopDownToDiag = 348,

        diagDown25Brakes = 349,

        count = 350,

        none = 65535,
    };
}

enum class SequenceFlag : uint8_t
{
    entranceConnectionNE, // Ride Entrances and path connect to front
    entranceConnectionSE, // connect to right
    entranceConnectionSW, // connect to back
    entranceConnectionNW, // connect to left
    trackOrigin,
    connectsToPath,  // The sides of this sequence block flagged with entrance connections can connect directly to paths
    disallowDoors,   // Doors cannot be built on any side of this track element (only valid on first sequence of track element)
    hasHeightMarker, // Displays a height marker on this sequence block
};

struct TrackCircuitIterator
{
    CoordsXYE last;
    CoordsXYE current;
    int32_t currentZ;
    int32_t currentDirection;
    OpenRCT2::TileElement* first;
    bool firstIteration;
    bool looped;
};

struct TypeOrCurve
{
    bool isTrackType = true; // true if a OpenRCT2::TrackElemType is selected, false if a TrackCurve is selected;
    union
    {
        OpenRCT2::TrackElemType trackType = OpenRCT2::TrackElemType::none;
        TrackCurve curve;
    };

    constexpr TypeOrCurve() noexcept
    {
        isTrackType = false;
        curve = TrackCurve::none;
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

int32_t TrackIsConnectedByShape(OpenRCT2::TileElement* a, OpenRCT2::TileElement* b);

void TrackCircuitIteratorBegin(TrackCircuitIterator* it, CoordsXYE first);
bool TrackCircuitIteratorPrevious(TrackCircuitIterator* it);
bool TrackCircuitIteratorNext(TrackCircuitIterator* it);
bool TrackCircuitIteratorsMatch(const TrackCircuitIterator* firstIt, const TrackCircuitIterator* secondIt);

void TrackGetBack(const CoordsXYE& input, CoordsXYE* output);
void TrackGetFront(const CoordsXYE& input, CoordsXYE* output);

OpenRCT2::TrackElement* TrackGetPreviousBlock(CoordsXYZ& location, OpenRCT2::TileElement* tileElement);

bool TrackElementIsCovered(OpenRCT2::TrackElemType trackElementType);
OpenRCT2::TrackElemType UncoverTrackElement(OpenRCT2::TrackElemType trackElementType);
bool TrackTypeIsStation(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsBrakes(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsBlockBrakes(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsBooster(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsReverser(OpenRCT2::TrackElemType trackType);

TrackRoll TrackGetActualBank(OpenRCT2::TileElement* tileElement, TrackRoll bank);
TrackRoll TrackGetActualBank2(ride_type_t rideType, bool isInverted, TrackRoll bank);
TrackRoll TrackGetActualBank3(bool useInvertedSprites, OpenRCT2::TileElement* tileElement);

ResultWithMessage TrackAddStationElement(
    CoordsXYZD loc, RideId rideIndex, OpenRCT2::GameActions::CommandFlags flags, bool fromTrackDesign);
ResultWithMessage TrackRemoveStationElement(const CoordsXYZD& loc, RideId rideIndex, OpenRCT2::GameActions::CommandFlags flags);

bool TrackTypeHasSpeedSetting(OpenRCT2::TrackElemType trackType);
bool TrackTypeIsHelix(OpenRCT2::TrackElemType trackType);
std::optional<CoordsXYZD> GetTrackSegmentOrigin(const CoordsXYE& posEl);

constexpr bool TrackPieceDirectionIsDiagonal(const uint8_t direction)
{
    return direction & kTrackDirectionDiagonalMask;
}

bool TrackGetIsSheltered(const CoordsXYZ& input);
