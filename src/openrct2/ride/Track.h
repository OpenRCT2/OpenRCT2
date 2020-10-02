/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../object/Object.h"
#include "Ride.h"

constexpr const uint16_t RideConstructionSpecialPieceSelected = 0x100;

constexpr const int32_t BLOCK_BRAKE_BASE_SPEED = 0x20364;

using track_type_t = uint16_t;
using roll_type_t = uint8_t;
using pitch_type_t = uint8_t;

#pragma pack(push, 1)
struct rct_trackdefinition
{
    uint8_t type;
    pitch_type_t vangle_end;
    pitch_type_t vangle_start;
    roll_type_t bank_end;
    roll_type_t bank_start;
    int8_t preview_z_offset;
    uint8_t pad[2] = {};
};
assert_struct_size(rct_trackdefinition, 8);
#pragma pack(pop)

struct PitchAndRoll
{
    pitch_type_t Pitch;
    roll_type_t Roll;
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
struct rct_preview_track
{
    uint8_t index; // 0x00
    int16_t x;     // 0x01
    int16_t y;     // 0x03
    int16_t z;     // 0x05
    uint8_t var_07;
    QuarterTile var_08;
    uint8_t flags;
};

/* size 0x0A */
struct rct_track_coordinates
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
    TRACK_ELEMENT_FLAG_TERMINAL_STATION = 1 << 3,
    TD6_TRACK_ELEMENT_FLAG_INVERTED = 1 << 6,
};

enum
{
    TRACK_ELEMENT_FLAGS2_CHAIN_LIFT = 1 << 0,
    TRACK_ELEMENT_FLAGS2_INVERTED = 1 << 1,
    // Used for giga coaster
    TRACK_ELEMENT_FLAGS2_CABLE_LIFT = 1 << 2,
    TRACK_ELEMENT_FLAGS2_HIGHLIGHT = 1 << 3,
    TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT = 1 << 4,
    TRACK_ELEMENT_FLAGS2_BLOCK_BRAKE_CLOSED = 1 << 5,
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

#define MAX_STATION_PLATFORM_LENGTH 32
constexpr uint16_t const MAX_TRACK_HEIGHT = 254 * COORDS_Z_STEP;
constexpr uint8_t const DEFAULT_SEAT_ROTATION = 4;

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
    TRACK_SLOPE_STEEP,
    TRACK_SLOPE_LONG,
    TRACK_SLOPE_CURVE,
    TRACK_SLOPE_CURVE_STEEP,
    TRACK_S_BEND,
    TRACK_CURVE_VERY_SMALL,
    TRACK_CURVE_SMALL,
    TRACK_CURVE,
    TRACK_TWIST,
    TRACK_HALF_LOOP,
    TRACK_CORKSCREW,
    TRACK_TOWER_BASE,
    TRACK_HELIX_SMALL,
    TRACK_HELIX_LARGE,
    TRACK_HELIX_LARGE_UNBANKED,
    TRACK_BRAKES,
    TRACK_25,
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
    TRACK_42,
    TRACK_LIFT_HILL_CABLE,
    TRACK_LIFT_HILL_CURVED,
    TRACK_QUARTER_LOOP,
    TRACK_SPINNING_TUNNEL,
    TRACK_ROTATION_CONTROL_TOGGLE,
    TRACK_BOOSTER = TRACK_ROTATION_CONTROL_TOGGLE,
    TRACK_INLINE_TWIST_UNINVERTED,
    TRACK_INLINE_TWIST_INVERTED,
    TRACK_QUARTER_LOOP_UNINVERTED,
    TRACK_QUARTER_LOOP_INVERTED,
    TRACK_RAPIDS,
    TRACK_HALF_LOOP_UNINVERTED,
    TRACK_HALF_LOOP_INVERTED,

    TRACK_WATERFALL,
    TRACK_WHIRLPOOL,
    TRACK_BRAKE_FOR_DROP,
    TRACK_CORKSCREW_UNINVERTED,
    TRACK_CORKSCREW_INVERTED,
    TRACK_HEARTLINE_TRANSFER,
    TRACK_MINI_GOLF_HOLE,

    TRACK_GROUP_COUNT,
};

enum
{
    TRACK_CURVE_LEFT_VERY_SMALL = 5,
    TRACK_CURVE_LEFT_SMALL = 3,
    TRACK_CURVE_LEFT = 1,
    TRACK_CURVE_LEFT_LARGE = 7,
    TRACK_CURVE_NONE = 0,
    TRACK_CURVE_RIGHT_LARGE = 8,
    TRACK_CURVE_RIGHT = 2,
    TRACK_CURVE_RIGHT_SMALL = 4,
    TRACK_CURVE_RIGHT_VERY_SMALL = 6
};

enum
{
    TRACK_SLOPE_NONE = 0,
    TRACK_SLOPE_UP_25 = 2,
    TRACK_SLOPE_UP_60 = 4,
    TRACK_SLOPE_DOWN_25 = 6,
    TRACK_SLOPE_DOWN_60 = 8,
    TRACK_SLOPE_UP_90 = 10,
    TRACK_SLOPE_DOWN_90 = 18,

    TRACK_VANGLE_TOWER = 10,
    TRACK_VANGLE_REVERSE_FREEFALL = 10
};

enum
{
    TRACK_BANK_NONE = 0,
    TRACK_BANK_LEFT = 2,
    TRACK_BANK_RIGHT = 4,
    TRACK_BANK_UPSIDE_DOWN = 15,
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
};

namespace TrackElemType
{
    constexpr uint16_t Flat = 0;
    constexpr uint16_t EndStation = 1;
    constexpr uint16_t BeginStation = 2;
    constexpr uint16_t MiddleStation = 3;
    constexpr uint16_t Up25 = 4;
    constexpr uint16_t Up60 = 5;
    constexpr uint16_t FlatToUp25 = 6;
    constexpr uint16_t Up25ToUp60 = 7;
    constexpr uint16_t Up60ToUp25 = 8;
    constexpr uint16_t Up25ToFlat = 9;
    constexpr uint16_t Down25 = 10;
    constexpr uint16_t Down60 = 11;
    constexpr uint16_t FlatToDown25 = 12;
    constexpr uint16_t Down25ToDown60 = 13;
    constexpr uint16_t Down60ToDown25 = 14;
    constexpr uint16_t Down25ToFlat = 15;
    constexpr uint16_t LeftQuarterTurn5Tiles = 16;
    constexpr uint16_t RightQuarterTurn5Tiles = 17;
    constexpr uint16_t FlatToLeftBank = 18;
    constexpr uint16_t FlatToRightBank = 19;
    constexpr uint16_t LeftBankToFlat = 20;
    constexpr uint16_t RightBankToFlat = 21;
    constexpr uint16_t BankedLeftQuarterTurn5Tiles = 22;
    constexpr uint16_t BankedRightQuarterTurn5Tiles = 23;
    constexpr uint16_t LeftBankToUp25 = 24;
    constexpr uint16_t RightBankToUp25 = 25;
    constexpr uint16_t Up25ToLeftBank = 26;
    constexpr uint16_t Up25ToRightBank = 27;
    constexpr uint16_t LeftBankToDown25 = 28;
    constexpr uint16_t RightBankToDown25 = 29;
    constexpr uint16_t Down25ToLeftBank = 30;
    constexpr uint16_t Down25ToRightBank = 31;
    constexpr uint16_t LeftBank = 32;
    constexpr uint16_t RightBank = 33;
    constexpr uint16_t LeftQuarterTurn5TilesUp25 = 34;
    constexpr uint16_t RightQuarterTurn5TilesUp25 = 35;
    constexpr uint16_t LeftQuarterTurn5TilesDown25 = 36;
    constexpr uint16_t RightQuarterTurn5TilesDown25 = 37;
    constexpr uint16_t SBendLeft = 38;
    constexpr uint16_t SBendRight = 39;
    constexpr uint16_t LeftVerticalLoop = 40;
    constexpr uint16_t RightVerticalLoop = 41;
    constexpr uint16_t LeftQuarterTurn3Tiles = 42;
    constexpr uint16_t RightQuarterTurn3Tiles = 43;
    constexpr uint16_t LeftBankedQuarterTurn3Tiles = 44;
    constexpr uint16_t RightBankedQuarterTurn3Tiles = 45;
    constexpr uint16_t LeftQuarterTurn3TilesUp25 = 46;
    constexpr uint16_t RightQuarterTurn3TilesUp25 = 47;
    constexpr uint16_t LeftQuarterTurn3TilesDown25 = 48;
    constexpr uint16_t RightQuarterTurn3TilesDown25 = 49;
    constexpr uint16_t LeftQuarterTurn1Tile = 50;
    constexpr uint16_t RightQuarterTurn1Tile = 51;
    constexpr uint16_t LeftTwistDownToUp = 52;
    constexpr uint16_t RightTwistDownToUp = 53;
    constexpr uint16_t LeftTwistUpToDown = 54;
    constexpr uint16_t RightTwistUpToDown = 55;
    constexpr uint16_t HalfLoopUp = 56;
    constexpr uint16_t HalfLoopDown = 57;
    constexpr uint16_t LeftCorkscrewUp = 58;
    constexpr uint16_t RightCorkscrewUp = 59;
    constexpr uint16_t LeftCorkscrewDown = 60;
    constexpr uint16_t RightCorkscrewDown = 61;
    constexpr uint16_t FlatToUp60 = 62;
    constexpr uint16_t Up60ToFlat = 63;
    constexpr uint16_t FlatToDown60 = 64;
    constexpr uint16_t Down60ToFlat = 65;
    constexpr uint16_t TowerBase = 66;
    constexpr uint16_t TowerSection = 67;
    constexpr uint16_t FlatCovered = 68;
    constexpr uint16_t Up25Covered = 69;
    constexpr uint16_t Up60Covered = 70;
    constexpr uint16_t FlatToUp25Covered = 71;
    constexpr uint16_t Up25ToUp60Covered = 72;
    constexpr uint16_t Up60ToUp25Covered = 73;
    constexpr uint16_t Up25ToFlatCovered = 74;
    constexpr uint16_t Down25Covered = 75;
    constexpr uint16_t Down60Covered = 76;
    constexpr uint16_t FlatToDown25Covered = 77;
    constexpr uint16_t Down25ToDown60Covered = 78;
    constexpr uint16_t Down60ToDown25Covered = 79;
    constexpr uint16_t Down25ToFlatCovered = 80;
    constexpr uint16_t LeftQuarterTurn5TilesCovered = 81;
    constexpr uint16_t RightQuarterTurn5TilesCovered = 82;
    constexpr uint16_t SBendLeftCovered = 83;
    constexpr uint16_t SBendRightCovered = 84;
    constexpr uint16_t LeftQuarterTurn3TilesCovered = 85;
    constexpr uint16_t RightQuarterTurn3TilesCovered = 86;
    constexpr uint16_t LeftHalfBankedHelixUpSmall = 87;
    constexpr uint16_t RightHalfBankedHelixUpSmall = 88;
    constexpr uint16_t LeftHalfBankedHelixDownSmall = 89;
    constexpr uint16_t RightHalfBankedHelixDownSmall = 90;
    constexpr uint16_t LeftHalfBankedHelixUpLarge = 91;
    constexpr uint16_t RightHalfBankedHelixUpLarge = 92;
    constexpr uint16_t LeftHalfBankedHelixDownLarge = 93;
    constexpr uint16_t RightHalfBankedHelixDownLarge = 94;
    constexpr uint16_t LeftQuarterTurn1TileUp60 = 95;
    constexpr uint16_t RightQuarterTurn1TileUp60 = 96;
    constexpr uint16_t LeftQuarterTurn1TileDown60 = 97;
    constexpr uint16_t RightQuarterTurn1TileDown60 = 98;
    constexpr uint16_t Brakes = 99;
    constexpr uint16_t RotationControlToggle = 100;
    constexpr uint16_t Booster = 100;
    constexpr uint16_t Maze = 101;
    // Used by the multi-dimension coaster, as TD6 cannot handle index 255.
    constexpr uint16_t InvertedUp90ToFlatQuarterLoopAlias = 101;
    constexpr uint16_t LeftQuarterBankedHelixLargeUp = 102;
    constexpr uint16_t RightQuarterBankedHelixLargeUp = 103;
    constexpr uint16_t LeftQuarterBankedHelixLargeDown = 104;
    constexpr uint16_t RightQuarterBankedHelixLargeDown = 105;
    constexpr uint16_t LeftQuarterHelixLargeUp = 106;
    constexpr uint16_t RightQuarterHelixLargeUp = 107;
    constexpr uint16_t LeftQuarterHelixLargeDown = 108;
    constexpr uint16_t RightQuarterHelixLargeDown = 109;
    constexpr uint16_t Up25LeftBanked = 110;
    constexpr uint16_t Up25RightBanked = 111;
    constexpr uint16_t Waterfall = 112;
    constexpr uint16_t Rapids = 113;
    constexpr uint16_t OnRidePhoto = 114;
    constexpr uint16_t Down25LeftBanked = 115;
    constexpr uint16_t Down25RightBanked = 116;
    constexpr uint16_t Watersplash = 117;
    constexpr uint16_t FlatToUp60LongBase = 118;
    constexpr uint16_t Up60ToFlatLongBase = 119;
    constexpr uint16_t Whirlpool = 120;
    constexpr uint16_t Down60ToFlatLongBase = 121;
    constexpr uint16_t FlatToDown60LongBase = 122;
    constexpr uint16_t CableLiftHill = 123;
    constexpr uint16_t ReverseFreefallSlope = 124;
    constexpr uint16_t ReverseFreefallVertical = 125;
    constexpr uint16_t Up90 = 126;
    constexpr uint16_t Down90 = 127;
    constexpr uint16_t Up60ToUp90 = 128;
    constexpr uint16_t Down90ToDown60 = 129;
    constexpr uint16_t Up90ToUp60 = 130;
    constexpr uint16_t Down60ToDown90 = 131;
    constexpr uint16_t BrakeForDrop = 132;
    constexpr uint16_t LeftEighthToDiag = 133;
    constexpr uint16_t RightEighthToDiag = 134;
    constexpr uint16_t LeftEighthToOrthogonal = 135;
    constexpr uint16_t RightEighthToOrthogonal = 136;
    constexpr uint16_t LeftEighthBankToDiag = 137;
    constexpr uint16_t RightEighthBankToDiag = 138;
    constexpr uint16_t LeftEighthBankToOrthogonal = 139;
    constexpr uint16_t RightEighthBankToOrthogonal = 140;
    constexpr uint16_t DiagFlat = 141;
    constexpr uint16_t DiagUp25 = 142;
    constexpr uint16_t DiagUp60 = 143;
    constexpr uint16_t DiagFlatToUp25 = 144;
    constexpr uint16_t DiagUp25ToUp60 = 145;
    constexpr uint16_t DiagUp60ToUp25 = 146;
    constexpr uint16_t DiagUp25ToFlat = 147;
    constexpr uint16_t DiagDown25 = 148;
    constexpr uint16_t DiagDown60 = 149;
    constexpr uint16_t DiagFlatToDown25 = 150;
    constexpr uint16_t DiagDown25ToDown60 = 151;
    constexpr uint16_t DiagDown60ToDown25 = 152;
    constexpr uint16_t DiagDown25ToFlat = 153;
    constexpr uint16_t DiagFlatToUp60 = 154;
    constexpr uint16_t DiagUp60ToFlat = 155;
    constexpr uint16_t DiagFlatToDown60 = 156;
    constexpr uint16_t DiagDown60ToFlat = 157;
    constexpr uint16_t DiagFlatToLeftBank = 158;
    constexpr uint16_t DiagFlatToRightBank = 159;
    constexpr uint16_t DiagLeftBankToFlat = 160;
    constexpr uint16_t DiagRightBankToFlat = 161;
    constexpr uint16_t DiagLeftBankToUp25 = 162;
    constexpr uint16_t DiagRightBankToUp25 = 163;
    constexpr uint16_t DiagUp25ToLeftBank = 164;
    constexpr uint16_t DiagUp25ToRightBank = 165;
    constexpr uint16_t DiagLeftBankToDown25 = 166;
    constexpr uint16_t DiagRightBankToDown25 = 167;
    constexpr uint16_t DiagDown25ToLeftBank = 168;
    constexpr uint16_t DiagDown25ToRightBank = 169;
    constexpr uint16_t DiagLeftBank = 170;
    constexpr uint16_t DiagRightBank = 171;
    constexpr uint16_t LogFlumeReverser = 172;
    constexpr uint16_t SpinningTunnel = 173;
    constexpr uint16_t LeftBarrelRollUpToDown = 174;
    constexpr uint16_t RightBarrelRollUpToDown = 175;
    constexpr uint16_t LeftBarrelRollDownToUp = 176;
    constexpr uint16_t RightBarrelRollDownToUp = 177;
    constexpr uint16_t LeftBankToLeftQuarterTurn3TilesUp25 = 178;
    constexpr uint16_t RightBankToRightQuarterTurn3TilesUp25 = 179;
    constexpr uint16_t LeftQuarterTurn3TilesDown25ToLeftBank = 180;
    constexpr uint16_t RightQuarterTurn3TilesDown25ToRightBank = 181;
    constexpr uint16_t PoweredLift = 182;
    constexpr uint16_t LeftLargeHalfLoopUp = 183;
    constexpr uint16_t RightLargeHalfLoopUp = 184;
    constexpr uint16_t RightLargeHalfLoopDown = 185;
    constexpr uint16_t LeftLargeHalfLoopDown = 186;
    constexpr uint16_t LeftFlyerTwistUp = 187;
    constexpr uint16_t RightFlyerTwistUp = 188;
    constexpr uint16_t LeftFlyerTwistDown = 189;
    constexpr uint16_t RightFlyerTwistDown = 190;
    constexpr uint16_t FlyerHalfLoopUp = 191;
    constexpr uint16_t FlyerHalfLoopDown = 192;
    constexpr uint16_t LeftFlyerCorkscrewUp = 193;
    constexpr uint16_t RightFlyerCorkscrewUp = 194;
    constexpr uint16_t LeftFlyerCorkscrewDown = 195;
    constexpr uint16_t RightFlyerCorkscrewDown = 196;
    constexpr uint16_t HeartLineTransferUp = 197;
    constexpr uint16_t HeartLineTransferDown = 198;
    constexpr uint16_t LeftHeartLineRoll = 199;
    constexpr uint16_t RightHeartLineRoll = 200;
    constexpr uint16_t MinigolfHoleA = 201;
    constexpr uint16_t MinigolfHoleB = 202;
    constexpr uint16_t MinigolfHoleC = 203;
    constexpr uint16_t MinigolfHoleD = 204;
    constexpr uint16_t MinigolfHoleE = 205;
    constexpr uint16_t MultiDimInvertedFlatToDown90QuarterLoop = 206;
    constexpr uint16_t Up90ToInvertedFlatQuarterLoop = 207;
    constexpr uint16_t InvertedFlatToDown90QuarterLoop = 208;
    constexpr uint16_t LeftCurvedLiftHill = 209;
    constexpr uint16_t RightCurvedLiftHill = 210;
    constexpr uint16_t LeftReverser = 211;
    constexpr uint16_t RightReverser = 212;
    constexpr uint16_t AirThrustTopCap = 213;
    constexpr uint16_t AirThrustVerticalDown = 214;
    constexpr uint16_t AirThrustVerticalDownToLevel = 215;
    constexpr uint16_t BlockBrakes = 216;
    constexpr uint16_t LeftBankedQuarterTurn3TileUp25 = 217;
    constexpr uint16_t RightBankedQuarterTurn3TileUp25 = 218;
    constexpr uint16_t LeftBankedQuarterTurn3TileDown25 = 219;
    constexpr uint16_t RightBankedQuarterTurn3TileDown25 = 220;
    constexpr uint16_t LeftBankedQuarterTurn5TileUp25 = 221;
    constexpr uint16_t RightBankedQuarterTurn5TileUp25 = 222;
    constexpr uint16_t LeftBankedQuarterTurn5TileDown25 = 223;
    constexpr uint16_t RightBankedQuarterTurn5TileDown25 = 224;
    constexpr uint16_t Up25ToLeftBankedUp25 = 225;
    constexpr uint16_t Up25ToRightBankedUp25 = 226;
    constexpr uint16_t LeftBankedUp25ToUp25 = 227;
    constexpr uint16_t RightBankedUp25ToUp25 = 228;
    constexpr uint16_t Down25ToLeftBankedDown25 = 229;
    constexpr uint16_t Down25ToRightBankedDown25 = 230;
    constexpr uint16_t LeftBankedDown25ToDown25 = 231;
    constexpr uint16_t RightBankedDown25ToDown25 = 232;
    constexpr uint16_t LeftBankedFlatToLeftBankedUp25 = 233;
    constexpr uint16_t RightBankedFlatToRightBankedUp25 = 234;
    constexpr uint16_t LeftBankedUp25ToLeftBankedFlat = 235;
    constexpr uint16_t RightBankedUp25ToRightBankedFlat = 236;
    constexpr uint16_t LeftBankedFlatToLeftBankedDown25 = 237;
    constexpr uint16_t RightBankedFlatToRightBankedDown25 = 238;
    constexpr uint16_t LeftBankedDown25ToLeftBankedFlat = 239;
    constexpr uint16_t RightBankedDown25ToRightBankedFlat = 240;
    constexpr uint16_t FlatToLeftBankedUp25 = 241;
    constexpr uint16_t FlatToRightBankedUp25 = 242;
    constexpr uint16_t LeftBankedUp25ToFlat = 243;
    constexpr uint16_t RightBankedUp25ToFlat = 244;
    constexpr uint16_t FlatToLeftBankedDown25 = 245;
    constexpr uint16_t FlatToRightBankedDown25 = 246;
    constexpr uint16_t LeftBankedDown25ToFlat = 247;
    constexpr uint16_t RightBankedDown25ToFlat = 248;
    constexpr uint16_t LeftQuarterTurn1TileUp90 = 249;
    constexpr uint16_t RightQuarterTurn1TileUp90 = 250;
    constexpr uint16_t LeftQuarterTurn1TileDown90 = 251;
    constexpr uint16_t RightQuarterTurn1TileDown90 = 252;
    constexpr uint16_t MultiDimUp90ToInvertedFlatQuarterLoop = 253;
    constexpr uint16_t MultiDimFlatToDown90QuarterLoop = 254;
    constexpr uint16_t MultiDimInvertedUp90ToFlatQuarterLoop = 255;

    constexpr uint16_t Count = 256;
}; // namespace TrackElemType

enum
{
    FLAT_TRACK_ELEM_1_X_4_A = 95,
    FLAT_TRACK_ELEM_2_X_2 = 110,
    FLAT_TRACK_ELEM_4_X_4 = 111,
    FLAT_TRACK_ELEM_1_X_5 = 116,
    FLAT_TRACK_ELEM_1_X_1_A = 118,
    FLAT_TRACK_ELEM_1_X_4_B = 119,
    FLAT_TRACK_ELEM_1_X_1_B = 121,
    FLAT_TRACK_ELEM_1_X_4_C = 122,
    FLAT_TRACK_ELEM_3_X_3 = 123,
};

enum
{
    TRACK_SEQUENCE_FLAG_DIRECTION_0 = (1 << 0),
    TRACK_SEQUENCE_FLAG_DIRECTION_1 = (1 << 1),
    TRACK_SEQUENCE_FLAG_DIRECTION_2 = (1 << 2),
    TRACK_SEQUENCE_FLAG_DIRECTION_3 = (1 << 3),
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

struct track_circuit_iterator
{
    CoordsXYE last;
    CoordsXYE current;
    int32_t currentZ;
    int32_t currentDirection;
    TileElement* first;
    bool firstIteration;
    bool looped;
};

extern const rct_trackdefinition FlatRideTrackDefinitions[256];
extern const rct_trackdefinition TrackDefinitions[256];

PitchAndRoll TrackPitchAndRollStart(track_type_t trackType);
PitchAndRoll TrackPitchAndRollEnd(track_type_t trackType);

int32_t track_is_connected_by_shape(TileElement* a, TileElement* b);

const rct_preview_track* get_track_def_from_ride(Ride* ride, int32_t trackType);
const rct_preview_track* get_track_def_from_ride_index(ride_id_t rideIndex, int32_t trackType);
const rct_track_coordinates* get_track_coord_from_ride(Ride* ride, int32_t trackType);

void track_circuit_iterator_begin(track_circuit_iterator* it, CoordsXYE first);
bool track_circuit_iterator_previous(track_circuit_iterator* it);
bool track_circuit_iterator_next(track_circuit_iterator* it);
bool track_circuit_iterators_match(const track_circuit_iterator* firstIt, const track_circuit_iterator* secondIt);

void track_get_back(CoordsXYE* input, CoordsXYE* output);
void track_get_front(CoordsXYE* input, CoordsXYE* output);

bool track_element_is_covered(track_type_t trackElementType);
bool track_type_is_station(track_type_t trackType);

roll_type_t track_get_actual_bank(TileElement* tileElement, roll_type_t bank);
roll_type_t track_get_actual_bank_2(int32_t rideType, bool isInverted, roll_type_t bank);
roll_type_t track_get_actual_bank_3(bool useInvertedSprites, TileElement* tileElement);

bool track_add_station_element(CoordsXYZD loc, ride_id_t rideIndex, int32_t flags, bool fromTrackDesign);
bool track_remove_station_element(const CoordsXYZD& loc, ride_id_t rideIndex, int32_t flags);

money32 maze_set_track(
    uint16_t x, uint16_t y, uint16_t z, uint8_t flags, bool initialPlacement, uint8_t direction, ride_id_t rideIndex,
    uint8_t mode);

bool TrackTypeIsBooster(uint8_t rideType, track_type_t trackType);
bool TrackTypeHasSpeedSetting(track_type_t trackType);
