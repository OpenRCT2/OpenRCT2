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
#include "../object/Object.h"
#include "../world/Map.h"
#include "../world/TileElement.h"

#include <optional>

constexpr const uint32_t RideConstructionSpecialPieceSelected = 0x10000;

constexpr const int32_t BLOCK_BRAKE_BASE_SPEED = 0x20364;
constexpr const uint8_t RCT2DefaultBlockBrakeSpeed = 2;

using track_type_t = uint16_t;
using roll_type_t = uint8_t;
using pitch_type_t = uint8_t;

struct ResultWithMessage;

struct TrackDefinition
{
    track_type_t type;
    pitch_type_t vangle_end;
    pitch_type_t vangle_start;
    roll_type_t bank_end;
    roll_type_t bank_start;
    int8_t preview_z_offset;
};

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
struct PreviewTrack
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

#define MAX_STATION_PLATFORM_LENGTH 32
constexpr uint16_t const MAX_TRACK_HEIGHT = 254 * COORDS_Z_STEP;
constexpr uint8_t const DEFAULT_SEAT_ROTATION = 4;

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
    constexpr track_type_t Flat = 0;
    constexpr track_type_t EndStation = 1;
    constexpr track_type_t BeginStation = 2;
    constexpr track_type_t MiddleStation = 3;
    constexpr track_type_t Up25 = 4;
    constexpr track_type_t Up60 = 5;
    constexpr track_type_t FlatToUp25 = 6;
    constexpr track_type_t Up25ToUp60 = 7;
    constexpr track_type_t Up60ToUp25 = 8;
    constexpr track_type_t Up25ToFlat = 9;
    constexpr track_type_t Down25 = 10;
    constexpr track_type_t Down60 = 11;
    constexpr track_type_t FlatToDown25 = 12;
    constexpr track_type_t Down25ToDown60 = 13;
    constexpr track_type_t Down60ToDown25 = 14;
    constexpr track_type_t Down25ToFlat = 15;
    constexpr track_type_t LeftQuarterTurn5Tiles = 16;
    constexpr track_type_t RightQuarterTurn5Tiles = 17;
    constexpr track_type_t FlatToLeftBank = 18;
    constexpr track_type_t FlatToRightBank = 19;
    constexpr track_type_t LeftBankToFlat = 20;
    constexpr track_type_t RightBankToFlat = 21;
    constexpr track_type_t BankedLeftQuarterTurn5Tiles = 22;
    constexpr track_type_t BankedRightQuarterTurn5Tiles = 23;
    constexpr track_type_t LeftBankToUp25 = 24;
    constexpr track_type_t RightBankToUp25 = 25;
    constexpr track_type_t Up25ToLeftBank = 26;
    constexpr track_type_t Up25ToRightBank = 27;
    constexpr track_type_t LeftBankToDown25 = 28;
    constexpr track_type_t RightBankToDown25 = 29;
    constexpr track_type_t Down25ToLeftBank = 30;
    constexpr track_type_t Down25ToRightBank = 31;
    constexpr track_type_t LeftBank = 32;
    constexpr track_type_t RightBank = 33;
    constexpr track_type_t LeftQuarterTurn5TilesUp25 = 34;
    constexpr track_type_t RightQuarterTurn5TilesUp25 = 35;
    constexpr track_type_t LeftQuarterTurn5TilesDown25 = 36;
    constexpr track_type_t RightQuarterTurn5TilesDown25 = 37;
    constexpr track_type_t SBendLeft = 38;
    constexpr track_type_t SBendRight = 39;
    constexpr track_type_t LeftVerticalLoop = 40;
    constexpr track_type_t RightVerticalLoop = 41;
    constexpr track_type_t LeftQuarterTurn3Tiles = 42;
    constexpr track_type_t RightQuarterTurn3Tiles = 43;
    constexpr track_type_t LeftBankedQuarterTurn3Tiles = 44;
    constexpr track_type_t RightBankedQuarterTurn3Tiles = 45;
    constexpr track_type_t LeftQuarterTurn3TilesUp25 = 46;
    constexpr track_type_t RightQuarterTurn3TilesUp25 = 47;
    constexpr track_type_t LeftQuarterTurn3TilesDown25 = 48;
    constexpr track_type_t RightQuarterTurn3TilesDown25 = 49;
    constexpr track_type_t LeftQuarterTurn1Tile = 50;
    constexpr track_type_t RightQuarterTurn1Tile = 51;
    constexpr track_type_t LeftTwistDownToUp = 52;
    constexpr track_type_t RightTwistDownToUp = 53;
    constexpr track_type_t LeftTwistUpToDown = 54;
    constexpr track_type_t RightTwistUpToDown = 55;
    constexpr track_type_t HalfLoopUp = 56;
    constexpr track_type_t HalfLoopDown = 57;
    constexpr track_type_t LeftCorkscrewUp = 58;
    constexpr track_type_t RightCorkscrewUp = 59;
    constexpr track_type_t LeftCorkscrewDown = 60;
    constexpr track_type_t RightCorkscrewDown = 61;
    constexpr track_type_t FlatToUp60 = 62;
    constexpr track_type_t Up60ToFlat = 63;
    constexpr track_type_t FlatToDown60 = 64;
    constexpr track_type_t Down60ToFlat = 65;
    constexpr track_type_t TowerBase = 66;
    constexpr track_type_t TowerSection = 67;
    constexpr track_type_t FlatCovered = 68;
    constexpr track_type_t Up25Covered = 69;
    constexpr track_type_t Up60Covered = 70;
    constexpr track_type_t FlatToUp25Covered = 71;
    constexpr track_type_t Up25ToUp60Covered = 72;
    constexpr track_type_t Up60ToUp25Covered = 73;
    constexpr track_type_t Up25ToFlatCovered = 74;
    constexpr track_type_t Down25Covered = 75;
    constexpr track_type_t Down60Covered = 76;
    constexpr track_type_t FlatToDown25Covered = 77;
    constexpr track_type_t Down25ToDown60Covered = 78;
    constexpr track_type_t Down60ToDown25Covered = 79;
    constexpr track_type_t Down25ToFlatCovered = 80;
    constexpr track_type_t LeftQuarterTurn5TilesCovered = 81;
    constexpr track_type_t RightQuarterTurn5TilesCovered = 82;
    constexpr track_type_t SBendLeftCovered = 83;
    constexpr track_type_t SBendRightCovered = 84;
    constexpr track_type_t LeftQuarterTurn3TilesCovered = 85;
    constexpr track_type_t RightQuarterTurn3TilesCovered = 86;
    constexpr track_type_t LeftHalfBankedHelixUpSmall = 87;
    constexpr track_type_t RightHalfBankedHelixUpSmall = 88;
    constexpr track_type_t LeftHalfBankedHelixDownSmall = 89;
    constexpr track_type_t RightHalfBankedHelixDownSmall = 90;
    constexpr track_type_t LeftHalfBankedHelixUpLarge = 91;
    constexpr track_type_t RightHalfBankedHelixUpLarge = 92;
    constexpr track_type_t LeftHalfBankedHelixDownLarge = 93;
    constexpr track_type_t RightHalfBankedHelixDownLarge = 94;
    constexpr track_type_t LeftQuarterTurn1TileUp60 = 95;
    constexpr track_type_t RightQuarterTurn1TileUp60 = 96;
    constexpr track_type_t LeftQuarterTurn1TileDown60 = 97;
    constexpr track_type_t RightQuarterTurn1TileDown60 = 98;
    constexpr track_type_t Brakes = 99;
    constexpr track_type_t RotationControlToggleAlias = 100;
    constexpr track_type_t Booster = 100;
    constexpr track_type_t Maze = 101;
    constexpr track_type_t LeftQuarterBankedHelixLargeUp = 102;
    constexpr track_type_t RightQuarterBankedHelixLargeUp = 103;
    constexpr track_type_t LeftQuarterBankedHelixLargeDown = 104;
    constexpr track_type_t RightQuarterBankedHelixLargeDown = 105;
    constexpr track_type_t LeftQuarterHelixLargeUp = 106;
    constexpr track_type_t RightQuarterHelixLargeUp = 107;
    constexpr track_type_t LeftQuarterHelixLargeDown = 108;
    constexpr track_type_t RightQuarterHelixLargeDown = 109;
    constexpr track_type_t Up25LeftBanked = 110;
    constexpr track_type_t Up25RightBanked = 111;
    constexpr track_type_t Waterfall = 112;
    constexpr track_type_t Rapids = 113;
    constexpr track_type_t OnRidePhoto = 114;
    constexpr track_type_t Down25LeftBanked = 115;
    constexpr track_type_t Down25RightBanked = 116;
    constexpr track_type_t Watersplash = 117;
    constexpr track_type_t FlatToUp60LongBase = 118;
    constexpr track_type_t Up60ToFlatLongBase = 119;
    constexpr track_type_t Whirlpool = 120;
    constexpr track_type_t Down60ToFlatLongBase = 121;
    constexpr track_type_t FlatToDown60LongBase = 122;
    constexpr track_type_t CableLiftHill = 123;
    constexpr track_type_t ReverseFreefallSlope = 124;
    constexpr track_type_t ReverseFreefallVertical = 125;
    constexpr track_type_t Up90 = 126;
    constexpr track_type_t Down90 = 127;
    constexpr track_type_t Up60ToUp90 = 128;
    constexpr track_type_t Down90ToDown60 = 129;
    constexpr track_type_t Up90ToUp60 = 130;
    constexpr track_type_t Down60ToDown90 = 131;
    constexpr track_type_t BrakeForDrop = 132;
    constexpr track_type_t LeftEighthToDiag = 133;
    constexpr track_type_t RightEighthToDiag = 134;
    constexpr track_type_t LeftEighthToOrthogonal = 135;
    constexpr track_type_t RightEighthToOrthogonal = 136;
    constexpr track_type_t LeftEighthBankToDiag = 137;
    constexpr track_type_t RightEighthBankToDiag = 138;
    constexpr track_type_t LeftEighthBankToOrthogonal = 139;
    constexpr track_type_t RightEighthBankToOrthogonal = 140;
    constexpr track_type_t DiagFlat = 141;
    constexpr track_type_t DiagUp25 = 142;
    constexpr track_type_t DiagUp60 = 143;
    constexpr track_type_t DiagFlatToUp25 = 144;
    constexpr track_type_t DiagUp25ToUp60 = 145;
    constexpr track_type_t DiagUp60ToUp25 = 146;
    constexpr track_type_t DiagUp25ToFlat = 147;
    constexpr track_type_t DiagDown25 = 148;
    constexpr track_type_t DiagDown60 = 149;
    constexpr track_type_t DiagFlatToDown25 = 150;
    constexpr track_type_t DiagDown25ToDown60 = 151;
    constexpr track_type_t DiagDown60ToDown25 = 152;
    constexpr track_type_t DiagDown25ToFlat = 153;
    constexpr track_type_t DiagFlatToUp60 = 154;
    constexpr track_type_t DiagUp60ToFlat = 155;
    constexpr track_type_t DiagFlatToDown60 = 156;
    constexpr track_type_t DiagDown60ToFlat = 157;
    constexpr track_type_t DiagFlatToLeftBank = 158;
    constexpr track_type_t DiagFlatToRightBank = 159;
    constexpr track_type_t DiagLeftBankToFlat = 160;
    constexpr track_type_t DiagRightBankToFlat = 161;
    constexpr track_type_t DiagLeftBankToUp25 = 162;
    constexpr track_type_t DiagRightBankToUp25 = 163;
    constexpr track_type_t DiagUp25ToLeftBank = 164;
    constexpr track_type_t DiagUp25ToRightBank = 165;
    constexpr track_type_t DiagLeftBankToDown25 = 166;
    constexpr track_type_t DiagRightBankToDown25 = 167;
    constexpr track_type_t DiagDown25ToLeftBank = 168;
    constexpr track_type_t DiagDown25ToRightBank = 169;
    constexpr track_type_t DiagLeftBank = 170;
    constexpr track_type_t DiagRightBank = 171;
    constexpr track_type_t LogFlumeReverser = 172;
    constexpr track_type_t SpinningTunnel = 173;
    constexpr track_type_t LeftBarrelRollUpToDown = 174;
    constexpr track_type_t RightBarrelRollUpToDown = 175;
    constexpr track_type_t LeftBarrelRollDownToUp = 176;
    constexpr track_type_t RightBarrelRollDownToUp = 177;
    constexpr track_type_t LeftBankToLeftQuarterTurn3TilesUp25 = 178;
    constexpr track_type_t RightBankToRightQuarterTurn3TilesUp25 = 179;
    constexpr track_type_t LeftQuarterTurn3TilesDown25ToLeftBank = 180;
    constexpr track_type_t RightQuarterTurn3TilesDown25ToRightBank = 181;
    constexpr track_type_t PoweredLift = 182;
    constexpr track_type_t LeftLargeHalfLoopUp = 183;
    constexpr track_type_t RightLargeHalfLoopUp = 184;
    constexpr track_type_t RightLargeHalfLoopDown = 185;
    constexpr track_type_t LeftLargeHalfLoopDown = 186;
    constexpr track_type_t LeftFlyerTwistUp = 187;
    constexpr track_type_t RightFlyerTwistUp = 188;
    constexpr track_type_t LeftFlyerTwistDown = 189;
    constexpr track_type_t RightFlyerTwistDown = 190;
    constexpr track_type_t FlyerHalfLoopUninvertedUp = 191;
    constexpr track_type_t FlyerHalfLoopInvertedDown = 192;
    constexpr track_type_t LeftFlyerCorkscrewUp = 193;
    constexpr track_type_t RightFlyerCorkscrewUp = 194;
    constexpr track_type_t LeftFlyerCorkscrewDown = 195;
    constexpr track_type_t RightFlyerCorkscrewDown = 196;
    constexpr track_type_t HeartLineTransferUp = 197;
    constexpr track_type_t HeartLineTransferDown = 198;
    constexpr track_type_t LeftHeartLineRoll = 199;
    constexpr track_type_t RightHeartLineRoll = 200;
    constexpr track_type_t MinigolfHoleA = 201;
    constexpr track_type_t MinigolfHoleB = 202;
    constexpr track_type_t MinigolfHoleC = 203;
    constexpr track_type_t MinigolfHoleD = 204;
    constexpr track_type_t MinigolfHoleE = 205;
    constexpr track_type_t MultiDimInvertedFlatToDown90QuarterLoop = 206;
    constexpr track_type_t Up90ToInvertedFlatQuarterLoop = 207;
    constexpr track_type_t InvertedFlatToDown90QuarterLoop = 208;
    constexpr track_type_t LeftCurvedLiftHill = 209;
    constexpr track_type_t RightCurvedLiftHill = 210;
    constexpr track_type_t LeftReverser = 211;
    constexpr track_type_t RightReverser = 212;
    constexpr track_type_t AirThrustTopCap = 213;
    constexpr track_type_t AirThrustVerticalDown = 214;
    constexpr track_type_t AirThrustVerticalDownToLevel = 215;
    constexpr track_type_t BlockBrakes = 216;
    constexpr track_type_t LeftBankedQuarterTurn3TileUp25 = 217;
    constexpr track_type_t RightBankedQuarterTurn3TileUp25 = 218;
    constexpr track_type_t LeftBankedQuarterTurn3TileDown25 = 219;
    constexpr track_type_t RightBankedQuarterTurn3TileDown25 = 220;
    constexpr track_type_t LeftBankedQuarterTurn5TileUp25 = 221;
    constexpr track_type_t RightBankedQuarterTurn5TileUp25 = 222;
    constexpr track_type_t LeftBankedQuarterTurn5TileDown25 = 223;
    constexpr track_type_t RightBankedQuarterTurn5TileDown25 = 224;
    constexpr track_type_t Up25ToLeftBankedUp25 = 225;
    constexpr track_type_t Up25ToRightBankedUp25 = 226;
    constexpr track_type_t LeftBankedUp25ToUp25 = 227;
    constexpr track_type_t RightBankedUp25ToUp25 = 228;
    constexpr track_type_t Down25ToLeftBankedDown25 = 229;
    constexpr track_type_t Down25ToRightBankedDown25 = 230;
    constexpr track_type_t LeftBankedDown25ToDown25 = 231;
    constexpr track_type_t RightBankedDown25ToDown25 = 232;
    constexpr track_type_t LeftBankedFlatToLeftBankedUp25 = 233;
    constexpr track_type_t RightBankedFlatToRightBankedUp25 = 234;
    constexpr track_type_t LeftBankedUp25ToLeftBankedFlat = 235;
    constexpr track_type_t RightBankedUp25ToRightBankedFlat = 236;
    constexpr track_type_t LeftBankedFlatToLeftBankedDown25 = 237;
    constexpr track_type_t RightBankedFlatToRightBankedDown25 = 238;
    constexpr track_type_t LeftBankedDown25ToLeftBankedFlat = 239;
    constexpr track_type_t RightBankedDown25ToRightBankedFlat = 240;
    constexpr track_type_t FlatToLeftBankedUp25 = 241;
    constexpr track_type_t FlatToRightBankedUp25 = 242;
    constexpr track_type_t LeftBankedUp25ToFlat = 243;
    constexpr track_type_t RightBankedUp25ToFlat = 244;
    constexpr track_type_t FlatToLeftBankedDown25 = 245;
    constexpr track_type_t FlatToRightBankedDown25 = 246;
    constexpr track_type_t LeftBankedDown25ToFlat = 247;
    constexpr track_type_t RightBankedDown25ToFlat = 248;
    constexpr track_type_t LeftQuarterTurn1TileUp90 = 249;
    constexpr track_type_t RightQuarterTurn1TileUp90 = 250;
    constexpr track_type_t LeftQuarterTurn1TileDown90 = 251;
    constexpr track_type_t RightQuarterTurn1TileDown90 = 252;
    constexpr track_type_t MultiDimUp90ToInvertedFlatQuarterLoop = 253;
    constexpr track_type_t MultiDimFlatToDown90QuarterLoop = 254;
    constexpr track_type_t MultiDimInvertedUp90ToFlatQuarterLoop = 255;
    constexpr track_type_t RotationControlToggle = 256;

    constexpr track_type_t FlatTrack1x4A = 257;
    constexpr track_type_t FlatTrack2x2 = 258;
    constexpr track_type_t FlatTrack4x4 = 259;
    constexpr track_type_t FlatTrack2x4 = 260;
    constexpr track_type_t FlatTrack1x5 = 261;
    constexpr track_type_t FlatTrack1x1A = 262;
    constexpr track_type_t FlatTrack1x4B = 263;
    constexpr track_type_t FlatTrack1x1B = 264;
    constexpr track_type_t FlatTrack1x4C = 265;
    constexpr track_type_t FlatTrack3x3 = 266;

    // SV6/TD6 element aliases
    constexpr track_type_t InvertedUp90ToFlatQuarterLoopAlias = 101;
    constexpr track_type_t FlatTrack1x4A_Alias = 95;
    constexpr track_type_t FlatTrack2x2_Alias = 110;
    constexpr track_type_t FlatTrack4x4_Alias = 111;
    constexpr track_type_t FlatTrack2x4_Alias = 115;
    constexpr track_type_t FlatTrack1x5_Alias = 116;
    constexpr track_type_t FlatTrack1x1A_Alias = 118;
    constexpr track_type_t FlatTrack1x4B_Alias = 119;
    constexpr track_type_t FlatTrack1x1B_Alias = 121;
    constexpr track_type_t FlatTrack1x4C_Alias = 122;
    constexpr track_type_t FlatTrack3x3_Alias = 123;

    // Highest track element ID that has a TD6 alias
    constexpr track_type_t HighestAlias = 266;

    // Track Elements specific to OpenRCT2
    constexpr track_type_t LeftLargeCorkscrewUp = 267;
    constexpr track_type_t RightLargeCorkscrewUp = 268;
    constexpr track_type_t LeftLargeCorkscrewDown = 269;
    constexpr track_type_t RightLargeCorkscrewDown = 270;
    constexpr track_type_t LeftMediumHalfLoopUp = 271;
    constexpr track_type_t RightMediumHalfLoopUp = 272;
    constexpr track_type_t LeftMediumHalfLoopDown = 273;
    constexpr track_type_t RightMediumHalfLoopDown = 274;
    constexpr track_type_t LeftZeroGRollUp = 275;
    constexpr track_type_t RightZeroGRollUp = 276;
    constexpr track_type_t LeftZeroGRollDown = 277;
    constexpr track_type_t RightZeroGRollDown = 278;
    constexpr track_type_t LeftLargeZeroGRollUp = 279;
    constexpr track_type_t RightLargeZeroGRollUp = 280;
    constexpr track_type_t LeftLargeZeroGRollDown = 281;
    constexpr track_type_t RightLargeZeroGRollDown = 282;

    constexpr track_type_t LeftFlyerLargeHalfLoopUninvertedUp = 283;
    constexpr track_type_t RightFlyerLargeHalfLoopUninvertedUp = 284;
    constexpr track_type_t RightFlyerLargeHalfLoopInvertedDown = 285;
    constexpr track_type_t LeftFlyerLargeHalfLoopInvertedDown = 286;
    constexpr track_type_t LeftFlyerLargeHalfLoopInvertedUp = 287;
    constexpr track_type_t RightFlyerLargeHalfLoopInvertedUp = 288;
    constexpr track_type_t RightFlyerLargeHalfLoopUninvertedDown = 289;
    constexpr track_type_t LeftFlyerLargeHalfLoopUninvertedDown = 290;

    constexpr track_type_t FlyerHalfLoopInvertedUp = 291;
    constexpr track_type_t FlyerHalfLoopUninvertedDown = 292;

    constexpr track_type_t Count = 293;
    constexpr track_type_t None = 65535;

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
bool TrackTypeIsStation(track_type_t trackType);

roll_type_t TrackGetActualBank(TileElement* tileElement, roll_type_t bank);
roll_type_t TrackGetActualBank2(int32_t rideType, bool isInverted, roll_type_t bank);
roll_type_t TrackGetActualBank3(bool useInvertedSprites, TileElement* tileElement);

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
