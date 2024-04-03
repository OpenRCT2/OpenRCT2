/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../object/Object.h"
#include "../world/Map.h"
#include "../world/TileElement.h"

#include <optional>

constexpr uint32_t kRideConstructionSpecialPieceSelected = 0x10000;

constexpr uint8_t kRCT2DefaultBlockBrakeSpeed = 2;
constexpr int32_t kBlockBrakeBaseSpeed = 0x20364;
constexpr int32_t kBlockBrakeSpeedOffset = kBlockBrakeBaseSpeed - (kRCT2DefaultBlockBrakeSpeed << 16);

using track_type_t = uint16_t;

struct ResultWithMessage;

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

struct TrackDefinition
{
    track_type_t Type;
    TrackPitch PitchEnd;
    TrackPitch PitchStart;
    TrackRoll RollEnd;
    TrackRoll RollStart;
    int8_t PreviewZOffset;
};

struct PitchAndRoll
{
    TrackPitch Pitch;
    TrackRoll Roll;
};
constexpr bool operator==(const PitchAndRoll& vb1, const PitchAndRoll& vb2)
{
    return vb1.Pitch == vb2.Pitch && vb1.Roll == vb2.Roll;
}
constexpr bool operator!=(const PitchAndRoll& vb1, const PitchAndRoll& vb2)
{
    return !(vb1 == vb2);
}

/* size 0x0A */
struct PreviewTrack
{
    uint8_t index; // 0x00
    int16_t x;     // 0x01
    int16_t y;     // 0x03
    int16_t z;     // 0x05
    uint8_t ClearanceZ;
    QuarterTile var_08;
    uint8_t flags;
};

/* size 0x0A */
struct TrackCoordinates
{
    int8_t rotation_begin; // 0x00
    int8_t rotation_end;   // 0x01
    int16_t z_begin;       // 0x02
    int16_t z_end;         // 0x04
    int16_t x;             // 0x06
    int16_t y;             // 0x08
};

enum
{
    RCT_PREVIEW_TRACK_FLAG_0 = (1 << 0),
    RCT_PREVIEW_TRACK_FLAG_1 = (1 << 1),
    RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL = (1 << 2),
};

enum
{
    TRACK_ELEMENT_FLAGS2_CHAIN_LIFT = 1 << 0,
    TRACK_ELEMENT_FLAGS2_INVERTED = 1 << 1,
    // Used for giga coaster
    TRACK_ELEMENT_FLAGS2_CABLE_LIFT = 1 << 2,
    TRACK_ELEMENT_FLAGS2_HIGHLIGHT = 1 << 3,
    TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT = 1 << 4,
    TRACK_ELEMENT_FLAGS2_BRAKE_CLOSED = 1 << 5,
    TRACK_ELEMENT_FLAGS2_INDESTRUCTIBLE_TRACK_PIECE = 1 << 6,
};

enum
{
    TRACK_ELEMENT_COLOUR_SCHEME_MASK = 0b00000011,
    // Not colour related, but shares the field.
    TRACK_ELEMENT_COLOUR_DOOR_A_MASK = 0b00011100,
    TRACK_ELEMENT_COLOUR_DOOR_B_MASK = 0b11100000,
    TRACK_ELEMENT_COLOUR_SEAT_ROTATION_MASK = 0b11110000,
};

constexpr int8_t kMaxStationPlatformLength = 32;
constexpr uint16_t const kMaxTrackHeight = 254 * COORDS_Z_STEP;
constexpr uint8_t const kDefaultSeatRotation = 4;

// Vehicle sprite groups required by track groups are defined in ride_entry_get_supported_track_pieces
enum
{
    TRACK_NONE = 0,

    TRACK_FLAT = 0,
    TRACK_STRAIGHT,
    TRACK_STATION_END,
    TRACK_LIFT_HILL,
    TRACK_LIFT_HILL_STEEP,
    TRACK_LIFT_HILL_CURVE,
    TRACK_FLAT_ROLL_BANKING,
    TRACK_VERTICAL_LOOP,
    TRACK_SLOPE,
    TRACK_SLOPE_STEEP_DOWN,
    TRACK_SLOPE_LONG,
    TRACK_SLOPE_CURVE,
    TRACK_SLOPE_CURVE_STEEP,
    TRACK_S_BEND,
    TRACK_CURVE_VERY_SMALL,
    TRACK_CURVE_SMALL,
    TRACK_CURVE,
    TRACK_CURVE_LARGE,
    TRACK_TWIST,
    TRACK_HALF_LOOP,
    TRACK_CORKSCREW,
    TRACK_TOWER_BASE,
    TRACK_HELIX_UP_BANKED_HALF,
    TRACK_HELIX_DOWN_BANKED_HALF,
    TRACK_HELIX_UP_BANKED_QUARTER,
    TRACK_HELIX_DOWN_BANKED_QUARTER,
    TRACK_HELIX_UP_UNBANKED_QUARTER,
    TRACK_HELIX_DOWN_UNBANKED_QUARTER,
    TRACK_BRAKES,
    TRACK_ON_RIDE_PHOTO,
    TRACK_WATER_SPLASH,
    TRACK_SLOPE_VERTICAL,
    TRACK_BARREL_ROLL,
    TRACK_POWERED_LIFT,
    TRACK_HALF_LOOP_LARGE,
    TRACK_SLOPE_CURVE_BANKED,
    TRACK_LOG_FLUME_REVERSER,
    TRACK_HEARTLINE_ROLL,
    TRACK_REVERSER,
    TRACK_REVERSE_FREEFALL,
    TRACK_SLOPE_TO_FLAT,
    TRACK_BLOCK_BRAKES,
    TRACK_SLOPE_ROLL_BANKING,
    TRACK_SLOPE_STEEP_LONG,
    TRACK_CURVE_VERTICAL,
    TRACK_LIFT_HILL_CABLE,
    TRACK_LIFT_HILL_CURVED,
    TRACK_QUARTER_LOOP,
    TRACK_SPINNING_TUNNEL,
    TRACK_BOOSTER,
    TRACK_INLINE_TWIST_UNINVERTED,
    TRACK_INLINE_TWIST_INVERTED,
    TRACK_QUARTER_LOOP_UNINVERTED_UP,
    TRACK_QUARTER_LOOP_UNINVERTED_DOWN,
    TRACK_QUARTER_LOOP_INVERTED_UP,
    TRACK_QUARTER_LOOP_INVERTED_DOWN,
    TRACK_RAPIDS,
    TRACK_FLYING_HALF_LOOP_UNINVERTED_UP,
    TRACK_FLYING_HALF_LOOP_INVERTED_DOWN,

    TRACK_FLAT_RIDE_BASE,

    TRACK_WATERFALL,
    TRACK_WHIRLPOOL,
    TRACK_BRAKE_FOR_DROP,
    TRACK_CORKSCREW_UNINVERTED,
    TRACK_CORKSCREW_INVERTED,
    TRACK_HEARTLINE_TRANSFER,
    TRACK_MINI_GOLF_HOLE,
    TRACK_ROTATION_CONTROL_TOGGLE,
    TRACK_SLOPE_STEEP_UP,

    TRACK_CORKSCREW_LARGE,
    TRACK_HALF_LOOP_MEDIUM,
    TRACK_ZERO_G_ROLL,
    TRACK_ZERO_G_ROLL_LARGE,

    TRACK_FLYING_LARGE_HALF_LOOP_UNINVERTED_UP,
    TRACK_FLYING_LARGE_HALF_LOOP_INVERTED_DOWN,
    TRACK_FLYING_LARGE_HALF_LOOP_UNINVERTED_DOWN,
    TRACK_FLYING_LARGE_HALF_LOOP_INVERTED_UP,

    TRACK_FLYING_HALF_LOOP_UNINVERTED_DOWN,
    TRACK_FLYING_HALF_LOOP_INVERTED_UP,

    TRACK_SLOPE_CURVE_LARGE,
    TRACK_SLOPE_CURVE_LARGE_BANKED,

    TRACK_DIAG_BRAKES,
    TRACK_DIAG_BLOCK_BRAKES,

    TRACK_GROUP_COUNT,
};

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

namespace TrackElemType
{
    constexpr track_type_t kFlat = 0;
    constexpr track_type_t kEndStation = 1;
    constexpr track_type_t kBeginStation = 2;
    constexpr track_type_t kMiddleStation = 3;
    constexpr track_type_t kUp25 = 4;
    constexpr track_type_t kUp60 = 5;
    constexpr track_type_t kFlatToUp25 = 6;
    constexpr track_type_t kUp25ToUp60 = 7;
    constexpr track_type_t kUp60ToUp25 = 8;
    constexpr track_type_t kUp25ToFlat = 9;
    constexpr track_type_t kDown25 = 10;
    constexpr track_type_t kDown60 = 11;
    constexpr track_type_t kFlatToDown25 = 12;
    constexpr track_type_t kDown25ToDown60 = 13;
    constexpr track_type_t kDown60ToDown25 = 14;
    constexpr track_type_t kDown25ToFlat = 15;
    constexpr track_type_t kLeftQuarterTurn5Tiles = 16;
    constexpr track_type_t kRightQuarterTurn5Tiles = 17;
    constexpr track_type_t kFlatToLeftBank = 18;
    constexpr track_type_t kFlatToRightBank = 19;
    constexpr track_type_t kLeftBankToFlat = 20;
    constexpr track_type_t kRightBankToFlat = 21;
    constexpr track_type_t kBankedLeftQuarterTurn5Tiles = 22;
    constexpr track_type_t kBankedRightQuarterTurn5Tiles = 23;
    constexpr track_type_t kLeftBankToUp25 = 24;
    constexpr track_type_t kRightBankToUp25 = 25;
    constexpr track_type_t kUp25ToLeftBank = 26;
    constexpr track_type_t kUp25ToRightBank = 27;
    constexpr track_type_t kLeftBankToDown25 = 28;
    constexpr track_type_t kRightBankToDown25 = 29;
    constexpr track_type_t kDown25ToLeftBank = 30;
    constexpr track_type_t kDown25ToRightBank = 31;
    constexpr track_type_t kLeftBank = 32;
    constexpr track_type_t kRightBank = 33;
    constexpr track_type_t kLeftQuarterTurn5TilesUp25 = 34;
    constexpr track_type_t kRightQuarterTurn5TilesUp25 = 35;
    constexpr track_type_t kLeftQuarterTurn5TilesDown25 = 36;
    constexpr track_type_t kRightQuarterTurn5TilesDown25 = 37;
    constexpr track_type_t kSBendLeft = 38;
    constexpr track_type_t kSBendRight = 39;
    constexpr track_type_t kLeftVerticalLoop = 40;
    constexpr track_type_t kRightVerticalLoop = 41;
    constexpr track_type_t kLeftQuarterTurn3Tiles = 42;
    constexpr track_type_t kRightQuarterTurn3Tiles = 43;
    constexpr track_type_t kLeftBankedQuarterTurn3Tiles = 44;
    constexpr track_type_t kRightBankedQuarterTurn3Tiles = 45;
    constexpr track_type_t kLeftQuarterTurn3TilesUp25 = 46;
    constexpr track_type_t kRightQuarterTurn3TilesUp25 = 47;
    constexpr track_type_t kLeftQuarterTurn3TilesDown25 = 48;
    constexpr track_type_t kRightQuarterTurn3TilesDown25 = 49;
    constexpr track_type_t kLeftQuarterTurn1Tile = 50;
    constexpr track_type_t kRightQuarterTurn1Tile = 51;
    constexpr track_type_t kLeftTwistDownToUp = 52;
    constexpr track_type_t kRightTwistDownToUp = 53;
    constexpr track_type_t kLeftTwistUpToDown = 54;
    constexpr track_type_t kRightTwistUpToDown = 55;
    constexpr track_type_t kHalfLoopUp = 56;
    constexpr track_type_t kHalfLoopDown = 57;
    constexpr track_type_t kLeftCorkscrewUp = 58;
    constexpr track_type_t kRightCorkscrewUp = 59;
    constexpr track_type_t kLeftCorkscrewDown = 60;
    constexpr track_type_t kRightCorkscrewDown = 61;
    constexpr track_type_t kFlatToUp60 = 62;
    constexpr track_type_t kUp60ToFlat = 63;
    constexpr track_type_t kFlatToDown60 = 64;
    constexpr track_type_t kDown60ToFlat = 65;
    constexpr track_type_t kTowerBase = 66;
    constexpr track_type_t kTowerSection = 67;
    constexpr track_type_t kFlatCovered = 68;
    constexpr track_type_t kUp25Covered = 69;
    constexpr track_type_t kUp60Covered = 70;
    constexpr track_type_t kFlatToUp25Covered = 71;
    constexpr track_type_t kUp25ToUp60Covered = 72;
    constexpr track_type_t kUp60ToUp25Covered = 73;
    constexpr track_type_t kUp25ToFlatCovered = 74;
    constexpr track_type_t kDown25Covered = 75;
    constexpr track_type_t kDown60Covered = 76;
    constexpr track_type_t kFlatToDown25Covered = 77;
    constexpr track_type_t kDown25ToDown60Covered = 78;
    constexpr track_type_t kDown60ToDown25Covered = 79;
    constexpr track_type_t kDown25ToFlatCovered = 80;
    constexpr track_type_t kLeftQuarterTurn5TilesCovered = 81;
    constexpr track_type_t kRightQuarterTurn5TilesCovered = 82;
    constexpr track_type_t kSBendLeftCovered = 83;
    constexpr track_type_t kSBendRightCovered = 84;
    constexpr track_type_t kLeftQuarterTurn3TilesCovered = 85;
    constexpr track_type_t kRightQuarterTurn3TilesCovered = 86;
    constexpr track_type_t kLeftHalfBankedHelixUpSmall = 87;
    constexpr track_type_t kRightHalfBankedHelixUpSmall = 88;
    constexpr track_type_t kLeftHalfBankedHelixDownSmall = 89;
    constexpr track_type_t kRightHalfBankedHelixDownSmall = 90;
    constexpr track_type_t kLeftHalfBankedHelixUpLarge = 91;
    constexpr track_type_t kRightHalfBankedHelixUpLarge = 92;
    constexpr track_type_t kLeftHalfBankedHelixDownLarge = 93;
    constexpr track_type_t kRightHalfBankedHelixDownLarge = 94;
    constexpr track_type_t kLeftQuarterTurn1TileUp60 = 95;
    constexpr track_type_t kRightQuarterTurn1TileUp60 = 96;
    constexpr track_type_t kLeftQuarterTurn1TileDown60 = 97;
    constexpr track_type_t kRightQuarterTurn1TileDown60 = 98;
    constexpr track_type_t kBrakes = 99;
    constexpr track_type_t kRotationControlToggleAlias = 100;
    constexpr track_type_t kBooster = 100;
    constexpr track_type_t kMaze = 101;
    constexpr track_type_t kLeftQuarterBankedHelixLargeUp = 102;
    constexpr track_type_t kRightQuarterBankedHelixLargeUp = 103;
    constexpr track_type_t kLeftQuarterBankedHelixLargeDown = 104;
    constexpr track_type_t kRightQuarterBankedHelixLargeDown = 105;
    constexpr track_type_t kLeftQuarterHelixLargeUp = 106;
    constexpr track_type_t kRightQuarterHelixLargeUp = 107;
    constexpr track_type_t kLeftQuarterHelixLargeDown = 108;
    constexpr track_type_t kRightQuarterHelixLargeDown = 109;
    constexpr track_type_t kUp25LeftBanked = 110;
    constexpr track_type_t kUp25RightBanked = 111;
    constexpr track_type_t kWaterfall = 112;
    constexpr track_type_t kRapids = 113;
    constexpr track_type_t kOnRidePhoto = 114;
    constexpr track_type_t kDown25LeftBanked = 115;
    constexpr track_type_t kDown25RightBanked = 116;
    constexpr track_type_t kWatersplash = 117;
    constexpr track_type_t kFlatToUp60LongBase = 118;
    constexpr track_type_t kUp60ToFlatLongBase = 119;
    constexpr track_type_t kWhirlpool = 120;
    constexpr track_type_t kDown60ToFlatLongBase = 121;
    constexpr track_type_t kFlatToDown60LongBase = 122;
    constexpr track_type_t kCableLiftHill = 123;
    constexpr track_type_t kReverseFreefallSlope = 124;
    constexpr track_type_t kReverseFreefallVertical = 125;
    constexpr track_type_t kUp90 = 126;
    constexpr track_type_t kDown90 = 127;
    constexpr track_type_t kUp60ToUp90 = 128;
    constexpr track_type_t kDown90ToDown60 = 129;
    constexpr track_type_t kUp90ToUp60 = 130;
    constexpr track_type_t kDown60ToDown90 = 131;
    constexpr track_type_t kBrakeForDrop = 132;
    constexpr track_type_t kLeftEighthToDiag = 133;
    constexpr track_type_t kRightEighthToDiag = 134;
    constexpr track_type_t kLeftEighthToOrthogonal = 135;
    constexpr track_type_t kRightEighthToOrthogonal = 136;
    constexpr track_type_t kLeftEighthBankToDiag = 137;
    constexpr track_type_t kRightEighthBankToDiag = 138;
    constexpr track_type_t kLeftEighthBankToOrthogonal = 139;
    constexpr track_type_t kRightEighthBankToOrthogonal = 140;
    constexpr track_type_t kDiagFlat = 141;
    constexpr track_type_t kDiagUp25 = 142;
    constexpr track_type_t kDiagUp60 = 143;
    constexpr track_type_t kDiagFlatToUp25 = 144;
    constexpr track_type_t kDiagUp25ToUp60 = 145;
    constexpr track_type_t kDiagUp60ToUp25 = 146;
    constexpr track_type_t kDiagUp25ToFlat = 147;
    constexpr track_type_t kDiagDown25 = 148;
    constexpr track_type_t kDiagDown60 = 149;
    constexpr track_type_t kDiagFlatToDown25 = 150;
    constexpr track_type_t kDiagDown25ToDown60 = 151;
    constexpr track_type_t kDiagDown60ToDown25 = 152;
    constexpr track_type_t kDiagDown25ToFlat = 153;
    constexpr track_type_t kDiagFlatToUp60 = 154;
    constexpr track_type_t kDiagUp60ToFlat = 155;
    constexpr track_type_t kDiagFlatToDown60 = 156;
    constexpr track_type_t kDiagDown60ToFlat = 157;
    constexpr track_type_t kDiagFlatToLeftBank = 158;
    constexpr track_type_t kDiagFlatToRightBank = 159;
    constexpr track_type_t kDiagLeftBankToFlat = 160;
    constexpr track_type_t kDiagRightBankToFlat = 161;
    constexpr track_type_t kDiagLeftBankToUp25 = 162;
    constexpr track_type_t kDiagRightBankToUp25 = 163;
    constexpr track_type_t kDiagUp25ToLeftBank = 164;
    constexpr track_type_t kDiagUp25ToRightBank = 165;
    constexpr track_type_t kDiagLeftBankToDown25 = 166;
    constexpr track_type_t kDiagRightBankToDown25 = 167;
    constexpr track_type_t kDiagDown25ToLeftBank = 168;
    constexpr track_type_t kDiagDown25ToRightBank = 169;
    constexpr track_type_t kDiagLeftBank = 170;
    constexpr track_type_t kDiagRightBank = 171;
    constexpr track_type_t kLogFlumeReverser = 172;
    constexpr track_type_t kSpinningTunnel = 173;
    constexpr track_type_t kLeftBarrelRollUpToDown = 174;
    constexpr track_type_t kRightBarrelRollUpToDown = 175;
    constexpr track_type_t kLeftBarrelRollDownToUp = 176;
    constexpr track_type_t kRightBarrelRollDownToUp = 177;
    constexpr track_type_t kLeftBankToLeftQuarterTurn3TilesUp25 = 178;
    constexpr track_type_t kRightBankToRightQuarterTurn3TilesUp25 = 179;
    constexpr track_type_t kLeftQuarterTurn3TilesDown25ToLeftBank = 180;
    constexpr track_type_t kRightQuarterTurn3TilesDown25ToRightBank = 181;
    constexpr track_type_t kPoweredLift = 182;
    constexpr track_type_t kLeftLargeHalfLoopUp = 183;
    constexpr track_type_t kRightLargeHalfLoopUp = 184;
    constexpr track_type_t kLeftLargeHalfLoopDown = 185;
    constexpr track_type_t kRightLargeHalfLoopDown = 186;
    constexpr track_type_t kLeftFlyerTwistUp = 187;
    constexpr track_type_t kRightFlyerTwistUp = 188;
    constexpr track_type_t kLeftFlyerTwistDown = 189;
    constexpr track_type_t kRightFlyerTwistDown = 190;
    constexpr track_type_t kFlyerHalfLoopUninvertedUp = 191;
    constexpr track_type_t kFlyerHalfLoopInvertedDown = 192;
    constexpr track_type_t kLeftFlyerCorkscrewUp = 193;
    constexpr track_type_t kRightFlyerCorkscrewUp = 194;
    constexpr track_type_t kLeftFlyerCorkscrewDown = 195;
    constexpr track_type_t kRightFlyerCorkscrewDown = 196;
    constexpr track_type_t kHeartLineTransferUp = 197;
    constexpr track_type_t kHeartLineTransferDown = 198;
    constexpr track_type_t kLeftHeartLineRoll = 199;
    constexpr track_type_t kRightHeartLineRoll = 200;
    constexpr track_type_t kMinigolfHoleA = 201;
    constexpr track_type_t kMinigolfHoleB = 202;
    constexpr track_type_t kMinigolfHoleC = 203;
    constexpr track_type_t kMinigolfHoleD = 204;
    constexpr track_type_t kMinigolfHoleE = 205;
    constexpr track_type_t kMultiDimInvertedFlatToDown90QuarterLoop = 206;
    constexpr track_type_t kUp90ToInvertedFlatQuarterLoop = 207;
    constexpr track_type_t kInvertedFlatToDown90QuarterLoop = 208;
    constexpr track_type_t kLeftCurvedLiftHill = 209;
    constexpr track_type_t kRightCurvedLiftHill = 210;
    constexpr track_type_t kLeftReverser = 211;
    constexpr track_type_t kRightReverser = 212;
    constexpr track_type_t kAirThrustTopCap = 213;
    constexpr track_type_t kAirThrustVerticalDown = 214;
    constexpr track_type_t kAirThrustVerticalDownToLevel = 215;
    constexpr track_type_t kBlockBrakes = 216;
    constexpr track_type_t kLeftBankedQuarterTurn3TileUp25 = 217;
    constexpr track_type_t kRightBankedQuarterTurn3TileUp25 = 218;
    constexpr track_type_t kLeftBankedQuarterTurn3TileDown25 = 219;
    constexpr track_type_t kRightBankedQuarterTurn3TileDown25 = 220;
    constexpr track_type_t kLeftBankedQuarterTurn5TileUp25 = 221;
    constexpr track_type_t kRightBankedQuarterTurn5TileUp25 = 222;
    constexpr track_type_t kLeftBankedQuarterTurn5TileDown25 = 223;
    constexpr track_type_t kRightBankedQuarterTurn5TileDown25 = 224;
    constexpr track_type_t kUp25ToLeftBankedUp25 = 225;
    constexpr track_type_t kUp25ToRightBankedUp25 = 226;
    constexpr track_type_t kLeftBankedUp25ToUp25 = 227;
    constexpr track_type_t kRightBankedUp25ToUp25 = 228;
    constexpr track_type_t kDown25ToLeftBankedDown25 = 229;
    constexpr track_type_t kDown25ToRightBankedDown25 = 230;
    constexpr track_type_t kLeftBankedDown25ToDown25 = 231;
    constexpr track_type_t kRightBankedDown25ToDown25 = 232;
    constexpr track_type_t kLeftBankedFlatToLeftBankedUp25 = 233;
    constexpr track_type_t kRightBankedFlatToRightBankedUp25 = 234;
    constexpr track_type_t kLeftBankedUp25ToLeftBankedFlat = 235;
    constexpr track_type_t kRightBankedUp25ToRightBankedFlat = 236;
    constexpr track_type_t kLeftBankedFlatToLeftBankedDown25 = 237;
    constexpr track_type_t kRightBankedFlatToRightBankedDown25 = 238;
    constexpr track_type_t kLeftBankedDown25ToLeftBankedFlat = 239;
    constexpr track_type_t kRightBankedDown25ToRightBankedFlat = 240;
    constexpr track_type_t kFlatToLeftBankedUp25 = 241;
    constexpr track_type_t kFlatToRightBankedUp25 = 242;
    constexpr track_type_t kLeftBankedUp25ToFlat = 243;
    constexpr track_type_t kRightBankedUp25ToFlat = 244;
    constexpr track_type_t kFlatToLeftBankedDown25 = 245;
    constexpr track_type_t kFlatToRightBankedDown25 = 246;
    constexpr track_type_t kLeftBankedDown25ToFlat = 247;
    constexpr track_type_t kRightBankedDown25ToFlat = 248;
    constexpr track_type_t kLeftQuarterTurn1TileUp90 = 249;
    constexpr track_type_t kRightQuarterTurn1TileUp90 = 250;
    constexpr track_type_t kLeftQuarterTurn1TileDown90 = 251;
    constexpr track_type_t kRightQuarterTurn1TileDown90 = 252;
    constexpr track_type_t kMultiDimUp90ToInvertedFlatQuarterLoop = 253;
    constexpr track_type_t kMultiDimFlatToDown90QuarterLoop = 254;
    constexpr track_type_t kMultiDimInvertedUp90ToFlatQuarterLoop = 255;
    constexpr track_type_t kRotationControlToggle = 256;

    constexpr track_type_t kFlatTrack1x4A = 257;
    constexpr track_type_t kFlatTrack2x2 = 258;
    constexpr track_type_t kFlatTrack4x4 = 259;
    constexpr track_type_t kFlatTrack2x4 = 260;
    constexpr track_type_t kFlatTrack1x5 = 261;
    constexpr track_type_t kFlatTrack1x1A = 262;
    constexpr track_type_t kFlatTrack1x4B = 263;
    constexpr track_type_t kFlatTrack1x1B = 264;
    constexpr track_type_t kFlatTrack1x4C = 265;
    constexpr track_type_t kFlatTrack3x3 = 266;

    // SV6/TD6 element aliases
    constexpr track_type_t kInvertedUp90ToFlatQuarterLoopAlias = 101;
    constexpr track_type_t kFlatTrack1x4A_Alias = 95;
    constexpr track_type_t kFlatTrack2x2_Alias = 110;
    constexpr track_type_t kFlatTrack4x4_Alias = 111;
    constexpr track_type_t kFlatTrack2x4_Alias = 115;
    constexpr track_type_t kFlatTrack1x5_Alias = 116;
    constexpr track_type_t kFlatTrack1x1A_Alias = 118;
    constexpr track_type_t kFlatTrack1x4B_Alias = 119;
    constexpr track_type_t kFlatTrack1x1B_Alias = 121;
    constexpr track_type_t kFlatTrack1x4C_Alias = 122;
    constexpr track_type_t kFlatTrack3x3_Alias = 123;

    // Highest track element ID that has a TD6 alias
    constexpr track_type_t kHighestAlias = 266;

    // Track Elements specific to OpenRCT2
    constexpr track_type_t kLeftLargeCorkscrewUp = 267;
    constexpr track_type_t kRightLargeCorkscrewUp = 268;
    constexpr track_type_t kLeftLargeCorkscrewDown = 269;
    constexpr track_type_t kRightLargeCorkscrewDown = 270;
    constexpr track_type_t kLeftMediumHalfLoopUp = 271;
    constexpr track_type_t kRightMediumHalfLoopUp = 272;
    constexpr track_type_t kLeftMediumHalfLoopDown = 273;
    constexpr track_type_t kRightMediumHalfLoopDown = 274;
    constexpr track_type_t kLeftZeroGRollUp = 275;
    constexpr track_type_t kRightZeroGRollUp = 276;
    constexpr track_type_t kLeftZeroGRollDown = 277;
    constexpr track_type_t kRightZeroGRollDown = 278;
    constexpr track_type_t kLeftLargeZeroGRollUp = 279;
    constexpr track_type_t kRightLargeZeroGRollUp = 280;
    constexpr track_type_t kLeftLargeZeroGRollDown = 281;
    constexpr track_type_t kRightLargeZeroGRollDown = 282;

    constexpr track_type_t kLeftFlyerLargeHalfLoopUninvertedUp = 283;
    constexpr track_type_t kRightFlyerLargeHalfLoopUninvertedUp = 284;
    constexpr track_type_t kLeftFlyerLargeHalfLoopInvertedDown = 285;
    constexpr track_type_t kRightFlyerLargeHalfLoopInvertedDown = 286;
    constexpr track_type_t kLeftFlyerLargeHalfLoopInvertedUp = 287;
    constexpr track_type_t kRightFlyerLargeHalfLoopInvertedUp = 288;
    constexpr track_type_t kLeftFlyerLargeHalfLoopUninvertedDown = 289;
    constexpr track_type_t kRightFlyerLargeHalfLoopUninvertedDown = 290;

    constexpr track_type_t kFlyerHalfLoopInvertedUp = 291;
    constexpr track_type_t kFlyerHalfLoopUninvertedDown = 292;

    constexpr track_type_t kLeftEighthToDiagUp25 = 293;
    constexpr track_type_t kRightEighthToDiagUp25 = 294;
    constexpr track_type_t kLeftEighthToDiagDown25 = 295;
    constexpr track_type_t kRightEighthToDiagDown25 = 296;
    constexpr track_type_t kLeftEighthToOrthogonalUp25 = 297;
    constexpr track_type_t kRightEighthToOrthogonalUp25 = 298;
    constexpr track_type_t kLeftEighthToOrthogonalDown25 = 299;
    constexpr track_type_t kRightEighthToOrthogonalDown25 = 300;

    constexpr track_type_t kDiagUp25ToLeftBankedUp25 = 301;
    constexpr track_type_t kDiagUp25ToRightBankedUp25 = 302;
    constexpr track_type_t kDiagLeftBankedUp25ToUp25 = 303;
    constexpr track_type_t kDiagRightBankedUp25ToUp25 = 304;
    constexpr track_type_t kDiagDown25ToLeftBankedDown25 = 305;
    constexpr track_type_t kDiagDown25ToRightBankedDown25 = 306;
    constexpr track_type_t kDiagLeftBankedDown25ToDown25 = 307;
    constexpr track_type_t kDiagRightBankedDown25ToDown25 = 308;
    constexpr track_type_t kDiagLeftBankedFlatToLeftBankedUp25 = 309;
    constexpr track_type_t kDiagRightBankedFlatToRightBankedUp25 = 310;
    constexpr track_type_t kDiagLeftBankedUp25ToLeftBankedFlat = 311;
    constexpr track_type_t kDiagRightBankedUp25ToRightBankedFlat = 312;
    constexpr track_type_t kDiagLeftBankedFlatToLeftBankedDown25 = 313;
    constexpr track_type_t kDiagRightBankedFlatToRightBankedDown25 = 314;
    constexpr track_type_t kDiagLeftBankedDown25ToLeftBankedFlat = 315;
    constexpr track_type_t kDiagRightBankedDown25ToRightBankedFlat = 316;
    constexpr track_type_t kDiagFlatToLeftBankedUp25 = 317;
    constexpr track_type_t kDiagFlatToRightBankedUp25 = 318;
    constexpr track_type_t kDiagLeftBankedUp25ToFlat = 319;
    constexpr track_type_t kDiagRightBankedUp25ToFlat = 320;
    constexpr track_type_t kDiagFlatToLeftBankedDown25 = 321;
    constexpr track_type_t kDiagFlatToRightBankedDown25 = 322;
    constexpr track_type_t kDiagLeftBankedDown25ToFlat = 323;
    constexpr track_type_t kDiagRightBankedDown25ToFlat = 324;
    constexpr track_type_t kDiagUp25LeftBanked = 325;
    constexpr track_type_t kDiagUp25RightBanked = 326;
    constexpr track_type_t kDiagDown25LeftBanked = 327;
    constexpr track_type_t kDiagDown25RightBanked = 328;

    constexpr track_type_t kLeftEighthBankToDiagUp25 = 329;
    constexpr track_type_t kRightEighthBankToDiagUp25 = 330;
    constexpr track_type_t kLeftEighthBankToDiagDown25 = 331;
    constexpr track_type_t kRightEighthBankToDiagDown25 = 332;
    constexpr track_type_t kLeftEighthBankToOrthogonalUp25 = 333;
    constexpr track_type_t kRightEighthBankToOrthogonalUp25 = 334;
    constexpr track_type_t kLeftEighthBankToOrthogonalDown25 = 335;
    constexpr track_type_t kRightEighthBankToOrthogonalDown25 = 336;

    constexpr track_type_t kDiagBrakes = 337;
    constexpr track_type_t kDiagBlockBrakes = 338;

    constexpr track_type_t kCount = 339;
    constexpr track_type_t kNone = 65535;

}; // namespace TrackElemType

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

PitchAndRoll TrackPitchAndRollStart(track_type_t trackType);
PitchAndRoll TrackPitchAndRollEnd(track_type_t trackType);

int32_t TrackIsConnectedByShape(TileElement* a, TileElement* b);

void TrackCircuitIteratorBegin(TrackCircuitIterator* it, CoordsXYE first);
bool TrackCircuitIteratorPrevious(TrackCircuitIterator* it);
bool TrackCircuitIteratorNext(TrackCircuitIterator* it);
bool TrackCircuitIteratorsMatch(const TrackCircuitIterator* firstIt, const TrackCircuitIterator* secondIt);

void TrackGetBack(const CoordsXYE& input, CoordsXYE* output);
void TrackGetFront(const CoordsXYE& input, CoordsXYE* output);

bool TrackElementIsCovered(track_type_t trackElementType);
track_type_t UncoverTrackElement(track_type_t trackElementType);
bool TrackTypeIsStation(track_type_t trackType);
bool TrackTypeIsBrakes(track_type_t trackType);
bool TrackTypeIsBlockBrakes(track_type_t trackType);
bool TrackTypeIsBooster(track_type_t trackType);

std::optional<CoordsXYZ> GetTrackElementOriginAndApplyChanges(
    const CoordsXYZD& location, track_type_t type, uint16_t extra_params, TileElement** output_element, uint16_t flags);

TrackRoll TrackGetActualBank(TileElement* tileElement, TrackRoll bank);
TrackRoll TrackGetActualBank2(int32_t rideType, bool isInverted, TrackRoll bank);
TrackRoll TrackGetActualBank3(bool useInvertedSprites, TileElement* tileElement);

ResultWithMessage TrackAddStationElement(CoordsXYZD loc, RideId rideIndex, int32_t flags, bool fromTrackDesign);
ResultWithMessage TrackRemoveStationElement(const CoordsXYZD& loc, RideId rideIndex, int32_t flags);

bool TrackTypeHasSpeedSetting(track_type_t trackType);
bool TrackTypeIsHelix(track_type_t trackType);
std::optional<CoordsXYZD> GetTrackSegmentOrigin(const CoordsXYE& posEl);

/**
 * If new pieces get added to existing ride types, this could cause existing parks to change appearance,
 * since the formerly unrendered pieces were not explicitly set invisible.
 * To avoid this, this function will return true if the piece is question was added after the park was created,
 * so that import code can properly set the visibility.
 *
 * @param rideType The OpenRCT2 ride type
 * @param trackType The OpenRCT2 track type
 * @param parkFileVersion The current park file version. Pass -1 when converting S4 or S6.
 * @return
 */
bool TrackTypeMustBeMadeInvisible(ride_type_t rideType, track_type_t trackType, int32_t parkFileVersion = -1);
