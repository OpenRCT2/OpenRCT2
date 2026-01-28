/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

// Structures shared between both RCT1 and RCT2.

#include "../core/EnumUtils.hpp"
#include "../core/FlagHolder.hpp"
#include "../core/Money.hpp"
#include "../entity/Fountain.h"
#include "../management/Research.h"
#include "../object/Object.h"
#include "../ride/RideTypes.h"
#include "../world/tile_element/TileElementType.h"
#include "Limits.h"

#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace OpenRCT2
{
    class ObjectList;
    enum class TrackElemType : uint16_t;
} // namespace OpenRCT2

namespace OpenRCT2::Drawing
{
    enum class TextColour : uint8_t;
}

namespace OpenRCT2::RCT12
{
    enum class ClimateType : uint8_t
    {
        coolAndWet,
        warm,
        hotAndDry,
        cold,
        count
    };

    class EntryList;

    enum class TrackElemType : uint8_t
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

        // SV6/TD6 element aliases
        rotationControlToggleAlias = 100,
        invertedUp90ToFlatQuarterLoopAlias = 101,
        flatTrack1x4A_Alias = 95,
        flatTrack2x2_Alias = 110,
        flatTrack4x4_Alias = 111,
        flatTrack2x4_Alias = 115,
        flatTrack1x5_Alias = 116,
        flatTrack1x1A_Alias = 118,
        flatTrack1x4B_Alias = 119,
        flatTrack1x1B_Alias = 121,
        flatTrack1x4C_Alias = 122,
        flatTrack3x3_Alias = 123,
    };
} // namespace OpenRCT2::RCT12

constexpr uint8_t kRCT12StringFormatArgStart = 123;
constexpr uint8_t kRCT12StringFormatArgEnd = 141;
constexpr uint8_t kRCT12StringFormatColourStart = 142;
constexpr uint8_t kRCT12StringFormatColourEnd = 156;

constexpr uint8_t kRCT12SoundIdNull = 0xFF;

using RCT12RideId = uint8_t;
constexpr RCT12RideId kRCT12RideIdNull = 255;

constexpr uint8_t kRCT12BannerIndexNull = std::numeric_limits<uint8_t>::max();

constexpr uint8_t kRCT12SurfaceElementEdgeStyleMask = 0xE0;         // in RCT12TileElement.properties.surface.slope
constexpr uint8_t kRCT12SurfaceElementWaterHeightMask = 0x1F;       // in RCT12TileElement.properties.surface.terrain
constexpr uint8_t kRCT12SurfaceElementTerrainMask = 0xE0;           // in RCT12TileElement.properties.surface.terrain
constexpr uint8_t kRCT12SurfaceElementTypeSurfaceMask = 0b00000011; // in RCT12TileElement.properties.surface.type
constexpr uint8_t kRCT12SurfaceElementTypeEdgeMask = 0b01000000;    // in RCT12TileElement.properties.surface.type

constexpr uint8_t kRCT12SmallSceneryElementNeedsSupportsFlag = 0x20;
constexpr uint8_t kRCT12TileElementColourMask = 0b0001'1111;

constexpr uint16_t kRCT12TileElementLargeTypeMask = 0x3FF;

constexpr uint8_t kRCT12TrackElementTypeFlagChainLift = 1 << 7;
constexpr uint8_t kRCT12TrackElementSequenceGreenLight = 1 << 7;

constexpr uint16_t kRCT12xy8Undefined = 0xFFFF;

using RCT12ObjectEntryIndex = uint8_t;
constexpr RCT12ObjectEntryIndex kRCT12ObjectEntryIndexNull = 255;

// Everything before this point has been researched
constexpr uint32_t kRCT12ResearchedItemsSeparator = 0xFFFFFFFF;
// Everything before this point and after separator still requires research
constexpr uint32_t kRCT12ResearchedItemsEnd = 0xFFFFFFFE;
// Extra end of list entry. Leftover from RCT1.
constexpr uint32_t kRCT12ResearchedItemsEnd2 = 0xFFFFFFFD;

constexpr uint16_t kRCT12PeepSpawnUndefined = 0xFFFF;

constexpr uint16_t kRCT12VehicleTrackDirectionMask = 0b0000000000000011;
constexpr uint16_t kRCT12VehicleTrackTypeMask = 0b1111111111111100;

constexpr uint8_t kRCT12PeepThoughtItemNone = std::numeric_limits<uint8_t>::max();

constexpr uint8_t kRCT12GuestsInParkHistoryFactor = 20;
constexpr uint8_t kRCT12ParkRatingHistoryFactor = 4;
constexpr uint8_t kRCT12ParkHistoryUndefined = std::numeric_limits<uint8_t>::max();

constexpr uint8_t kTD46RatingsMultiplier = 10;
constexpr uint8_t kTD46GForcesMultiplier = 32;

constexpr uint8_t kRCT12InversionAndHoleMask = 0b00011111;
constexpr uint8_t kRCT12RideNumDropsMask = 0b00111111;

enum class RideColourScheme : uint8_t;
enum class BannerFlag : uint8_t;
using BannerFlags = FlagHolder<uint8_t, BannerFlag>;

enum
{
    RCT12_TILE_ELEMENT_FLAG_GHOST = (1 << 4),
    RCT12_TILE_ELEMENT_FLAG_BROKEN = (1 << 5),
    RCT12_TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED = (1 << 5),
    RCT12_TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE = (1 << 6),
    RCT12_TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE = (1 << 6),
    RCT12_TILE_ELEMENT_FLAG_LARGE_SCENERY_ACCOUNTED = (1 << 6),
    RCT12_TILE_ELEMENT_FLAG_LAST_TILE = (1 << 7)
};

enum
{
    RCT12_TRACK_ELEMENT_SEQUENCE_STATION_INDEX_MASK = 0b01110000,
    RCT12_TRACK_ELEMENT_SEQUENCE_SEQUENCE_MASK = 0b00001111,
    RCT12_TRACK_ELEMENT_SEQUENCE_TAKING_PHOTO_MASK = 0b11110000,
};

enum
{
    // Not anything to do with colour but uses
    // that field in the tile element

    // Used for multi-dimension coaster
    RCT12_TRACK_ELEMENT_COLOUR_FLAG_INVERTED = (1 << 2),

    // Used for giga coaster
    RCT12_TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT = (1 << 3),

    RCT12_TRACK_ELEMENT_DOOR_A_MASK = 0b00011100,
    RCT12_TRACK_ELEMENT_DOOR_B_MASK = 0b11100000,
};

// Masks and Flags for values stored in TileElement.properties.path.type
enum
{
    RCT12_FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK = (1 << 0) | (1 << 1),
    RCT12_FOOTPATH_PROPERTIES_FLAG_IS_SLOPED = (1 << 2),
    RCT12_FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER = (1 << 3),
    RCT12_FOOTPATH_PROPERTIES_TYPE_MASK = (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7),
};

// Masks and Flags for values stored in RCT12TileElement.properties.path.additions
enum
{
    RCT12_FOOTPATH_PROPERTIES_ADDITIONS_TYPE_MASK = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3),
    // The most significant bit in this mask will always be zero, since rides can only have 4 stations
    RCT12_FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK = (1 << 4) | (1 << 5) | (1 << 6),
    RCT12_FOOTPATH_PROPERTIES_ADDITIONS_FLAG_GHOST = (1 << 7),
};

enum
{
    RCT12_STATION_STYLE_PLAIN,
    RCT12_STATION_STYLE_WOODEN,
    RCT12_STATION_STYLE_CANVAS_TENT,
    RCT12_STATION_STYLE_CASTLE_GREY,
    RCT12_STATION_STYLE_CASTLE_BROWN,
    RCT12_STATION_STYLE_JUNGLE,
    RCT12_STATION_STYLE_LOG_CABIN,
    RCT12_STATION_STYLE_CLASSICAL,
    RCT12_STATION_STYLE_ABSTRACT,
    RCT12_STATION_STYLE_SNOW,
    RCT12_STATION_STYLE_PAGODA,
    RCT12_STATION_STYLE_SPACE,

    RCT12_STATION_STYLE_INVISIBLE, // Added by OpenRCT2
};

enum
{
    RCT12_ENTITY_FLAGS_IS_CRASHED_VEHICLE_ENTITY = 1 << 7,
};

// Only written to in RCT2, not used in OpenRCT2. All of these are elements that had to be invented in RCT1.
enum : uint32_t
{
    TRACK_FLAGS_CONTAINS_VERTICAL_LOOP = (1 << 7),
    TRACK_FLAGS_CONTAINS_INLINE_TWIST = (1 << 17),
    TRACK_FLAGS_CONTAINS_HALF_LOOP = (1 << 18),
    TRACK_FLAGS_CONTAINS_CORKSCREW = (1 << 19),
    TRACK_FLAGS_CONTAINS_WATER_SPLASH = (1 << 27),
    TRACK_FLAGS_CONTAINS_BARREL_ROLL = (1 << 29),
    TRACK_FLAGS_CONTAINS_POWERED_LIFT = (1 << 30),
    TRACK_FLAGS_CONTAINS_LARGE_HALF_LOOP = (1u << 31),
};

enum : uint32_t
{
    TRACK_FLAGS2_CONTAINS_LOG_FLUME_REVERSER = (1 << 1),
    TRACK_FLAGS2_SIX_FLAGS_RIDE_DEPRECATED = (1u << 31) // Not used anymore.
};

static constexpr std::string_view kNoEntranceNoPlatformIdentifier = "openrct2.station.noplatformnoentrance";

constexpr std::string_view kDefaultStationStyles[] = {
    "rct2.station.plain",        // RCT12_STATION_STYLE_PLAIN
    "rct2.station.wooden",       // RCT12_STATION_STYLE_WOODEN
    "rct2.station.canvas_tent",  // RCT12_STATION_STYLE_CANVAS_TENT
    "rct2.station.castle_grey",  // RCT12_STATION_STYLE_CASTLE_GREY
    "rct2.station.castle_brown", // RCT12_STATION_STYLE_CASTLE_BROWN
    "rct2.station.jungle",       // RCT12_STATION_STYLE_JUNGLE
    "rct2.station.log",          // RCT12_STATION_STYLE_LOG_CABIN
    "rct2.station.classical",    // RCT12_STATION_STYLE_CLASSICAL
    "rct2.station.abstract",     // RCT12_STATION_STYLE_ABSTRACT
    "rct2.station.snow",         // RCT12_STATION_STYLE_SNOW
    "rct2.station.pagoda",       // RCT12_STATION_STYLE_PAGODA
    "rct2.station.space",        // RCT12_STATION_STYLE_SPACE
};

#pragma pack(push, 1)

struct RCT12xy8
{
    union
    {
        struct
        {
            uint8_t x, y;
        };
        uint16_t xy;
    };

    bool IsNull() const
    {
        return xy == kRCT12xy8Undefined;
    }

    void SetNull()
    {
        xy = kRCT12xy8Undefined;
    }
};
static_assert(sizeof(RCT12xy8) == 2);

/* Track Element entry  size: 0x02 */
struct TD46TrackElement
{
    OpenRCT2::RCT12::TrackElemType Type; // 0x00
    uint8_t Flags;                       // 0x01
};
static_assert(sizeof(TD46TrackElement) == 0x02);

struct RCT12Award
{
    uint16_t Time;
    uint16_t Type;
};
static_assert(sizeof(RCT12Award) == 4);

/**
 * A single news item / message.
 * size: 0x10C
 */
struct RCT12NewsItem
{
    uint8_t Type;
    uint8_t Flags;
    uint32_t Assoc;
    uint16_t Ticks;
    uint16_t MonthYear;
    uint8_t Day;
    uint8_t Pad0B;
    char Text[256];
};
static_assert(sizeof(RCT12NewsItem) == 0x10C);

struct RCT12xyzd8
{
    uint8_t x, y, z, direction;
};
static_assert(sizeof(RCT12xyzd8) == 4);

struct RCT12PeepSpawn
{
    uint16_t x;
    uint16_t y;
    uint8_t z;
    uint8_t direction;
};
static_assert(sizeof(RCT12PeepSpawn) == 6);

enum class RCT12TileElementType : uint8_t
{
    surface = 0,
    path = 1,
    track = 2,
    smallScenery = 3,
    entrance = 4,
    wall = 5,
    largeScenery = 6,
    banner = 7,
    corrupt = 8,
    eightCarsCorrupt14 = 14,
    eightCarsCorrupt15 = 15,
};

constexpr OpenRCT2::TileElementType ToOpenRCT2TileElementType(RCT12TileElementType rct12type)
{
    switch (rct12type)
    {
        case RCT12TileElementType::surface:
        case RCT12TileElementType::path:
        case RCT12TileElementType::track:
        case RCT12TileElementType::smallScenery:
        case RCT12TileElementType::entrance:
        case RCT12TileElementType::wall:
        case RCT12TileElementType::largeScenery:
        case RCT12TileElementType::banner:
            return static_cast<OpenRCT2::TileElementType>(rct12type);

        default:
            throw std::runtime_error(
                "This tile element type cannot be converted directly! Tile element type: "
                + std::to_string(static_cast<int>(rct12type)));
    }
}

struct RCT12SurfaceElement;
struct RCT12PathElement;
struct RCT12TrackElement;
struct RCT12SmallSceneryElement;
struct RCT12LargeSceneryElement;
struct RCT12WallElement;
struct RCT12EntranceElement;
struct RCT12BannerElement;
struct RCT12CorruptElement;
struct RCT12EightCarsCorruptElement14;
struct RCT12EightCarsCorruptElement15;

struct RCT12TileElementBase
{
    uint8_t Type;            // 0
    uint8_t Flags;           // 1. Upper nibble: Flags. Lower nibble: occupied quadrants (one bit per quadrant).
    uint8_t BaseHeight;      // 2
    uint8_t ClearanceHeight; // 3
    RCT12TileElementType GetType() const;
    uint8_t GetDirection() const;

    uint8_t GetOccupiedQuadrants() const;

    bool IsLastForTile() const;
    bool IsGhost() const;
};
/**
 * Map element structure
 * size: 0x08
 */
struct RCT12TileElement : public RCT12TileElementBase
{
    uint8_t Pad04[4];
    template<typename TType, RCT12TileElementType TClass>
    const TType* as() const
    {
        return GetType() == TClass ? reinterpret_cast<const TType*>(this) : nullptr;
    }
    template<typename TType, RCT12TileElementType TClass>
    TType* as()
    {
        return GetType() == TClass ? reinterpret_cast<TType*>(this) : nullptr;
    }

    const RCT12SurfaceElement* AsSurface() const
    {
        return as<RCT12SurfaceElement, RCT12TileElementType::surface>();
    }
    RCT12SurfaceElement* AsSurface()
    {
        return as<RCT12SurfaceElement, RCT12TileElementType::surface>();
    }
    const RCT12PathElement* AsPath() const
    {
        return as<RCT12PathElement, RCT12TileElementType::path>();
    }
    RCT12PathElement* AsPath()
    {
        return as<RCT12PathElement, RCT12TileElementType::path>();
    }
    const RCT12TrackElement* AsTrack() const
    {
        return as<RCT12TrackElement, RCT12TileElementType::track>();
    }
    RCT12TrackElement* AsTrack()
    {
        return as<RCT12TrackElement, RCT12TileElementType::track>();
    }
    const RCT12SmallSceneryElement* AsSmallScenery() const
    {
        return as<RCT12SmallSceneryElement, RCT12TileElementType::smallScenery>();
    }
    RCT12SmallSceneryElement* AsSmallScenery()
    {
        return as<RCT12SmallSceneryElement, RCT12TileElementType::smallScenery>();
    }
    const RCT12LargeSceneryElement* AsLargeScenery() const
    {
        return as<RCT12LargeSceneryElement, RCT12TileElementType::largeScenery>();
    }
    RCT12LargeSceneryElement* AsLargeScenery()
    {
        return as<RCT12LargeSceneryElement, RCT12TileElementType::largeScenery>();
    }
    const RCT12WallElement* AsWall() const
    {
        return as<RCT12WallElement, RCT12TileElementType::wall>();
    }
    RCT12WallElement* AsWall()
    {
        return as<RCT12WallElement, RCT12TileElementType::wall>();
    }
    const RCT12EntranceElement* AsEntrance() const
    {
        return as<RCT12EntranceElement, RCT12TileElementType::entrance>();
    }
    RCT12EntranceElement* AsEntrance()
    {
        return as<RCT12EntranceElement, RCT12TileElementType::entrance>();
    }
    const RCT12BannerElement* AsBanner() const
    {
        return as<RCT12BannerElement, RCT12TileElementType::banner>();
    }
    RCT12BannerElement* AsBanner()
    {
        return as<RCT12BannerElement, RCT12TileElementType::banner>();
    }
};
static_assert(sizeof(RCT12TileElement) == 8);
struct RCT12SurfaceElement : RCT12TileElementBase
{
private:
    uint8_t Slope;       // 4 0xE0 Edge Style, 0x1F Slope
    uint8_t Terrain;     // 5 0xE0 Terrain Style, 0x1F Water height
    uint8_t GrassLength; // 6
    uint8_t Ownership;   // 7
public:
    uint8_t GetSlope() const;
    uint32_t GetSurfaceStyle() const;
    uint32_t GetEdgeStyle() const;
    uint8_t GetGrassLength() const;
    uint8_t GetOwnership() const;
    uint32_t GetWaterHeight() const;
    uint8_t GetParkFences() const;
    bool HasTrackThatNeedsWater() const;
};
static_assert(sizeof(RCT12SurfaceElement) == 8);
struct RCT12PathElement : RCT12TileElementBase
{
private:
    uint8_t EntryIndex; // 4, 0xF0 Path type, 0x08 Ride sign, 0x04 Set when path is sloped, 0x03 Rotation
    uint8_t Additions;  // 5, 0bGSSSAAAA: G = Ghost, S = station index, A = addition (0 means no addition)
    uint8_t Edges;      // 6
    union
    {
        uint8_t AdditionStatus; // 7
        uint8_t RideIndex;
    };

public:
    RCT12ObjectEntryIndex GetEntryIndex() const;
    uint8_t GetQueueBannerDirection() const;
    bool IsSloped() const;
    uint8_t GetSlopeDirection() const;
    uint8_t GetRideIndex() const;
    uint8_t GetStationIndex() const;
    bool IsWide() const;
    bool IsQueue() const;
    bool HasQueueBanner() const;
    uint8_t GetEdges() const;
    uint8_t GetCorners() const;
    uint8_t GetAddition() const;
    bool AdditionIsGhost() const;
    uint8_t GetAdditionStatus() const;
    uint8_t GetRCT1PathType() const;
    uint8_t GetRCT1SupportType() const;

    bool IsBroken() const;

    bool IsBlockedByVehicle() const;
};
static_assert(sizeof(RCT12PathElement) == 8);
struct RCT12TrackElement : RCT12TileElementBase
{
private:
    OpenRCT2::RCT12::TrackElemType TrackType; // 4
    union
    {
        struct
        {
            // The lower 4 bits are the track sequence.
            // The upper 4 bits are either station bits, brake speed bits, or on-ride photo bits.
            //
            // Station bits:
            // - Bit 8 marks green light
            // - Bit 5-7 are station index.
            //
            // On-ride photo bits:
            // - Bits 7 and 8 are never set
            // - Bits 5 and 6 are set when a vehicle triggers the on-ride photo and act like a countdown from 3.
            // - If any of the bits 5-8 are set, the game counts it as a photo being taken.
            uint8_t Sequence; // 5.
            uint8_t Colour;   // 6
        };
        uint16_t MazeEntry; // 5
    };
    uint8_t RideIndex; // 7
public:
    OpenRCT2::RCT12::TrackElemType GetTrackType() const;
    uint8_t GetSequenceIndex() const;
    uint8_t GetRideIndex() const;
    RideColourScheme GetColourScheme() const;
    uint8_t GetStationIndex() const;
    bool HasChain() const;
    bool HasCableLift() const;
    bool IsInverted() const;
    uint8_t GetBrakeBoosterSpeed() const;
    bool HasGreenLight() const;
    uint8_t GetSeatRotation() const;
    uint16_t GetMazeEntry() const;
    uint8_t GetPhotoTimeout() const;
    // RCT1 feature, reintroduced by OpenRCT2. See https://github.com/OpenRCT2/OpenRCT2/issues/7059
    uint8_t GetDoorAState() const;
    uint8_t GetDoorBState() const;

    bool BlockBrakeClosed() const;
    bool IsIndestructible() const;
};
static_assert(sizeof(RCT12TrackElement) == 8);
struct RCT12SmallSceneryElement : RCT12TileElementBase
{
private:
    uint8_t EntryIndex; // 4
    uint8_t Age;        // 5
    uint8_t Colour1;    // 6
    uint8_t Colour2;    // 7
public:
    RCT12ObjectEntryIndex GetEntryIndex() const;
    uint8_t GetAge() const;
    uint8_t GetSceneryQuadrant() const;
    OpenRCT2::Drawing::Colour GetPrimaryColour() const;
    OpenRCT2::Drawing::Colour GetSecondaryColour() const;
    bool NeedsSupports() const;
};
static_assert(sizeof(RCT12SmallSceneryElement) == 8);
struct RCT12LargeSceneryElement : RCT12TileElementBase
{
private:
    uint16_t EntryIndex; // 4
    uint8_t Colour[2];   // 6
public:
    uint32_t GetEntryIndex() const;
    uint16_t GetSequenceIndex() const;
    OpenRCT2::Drawing::Colour GetPrimaryColour() const;
    OpenRCT2::Drawing::Colour GetSecondaryColour() const;
    uint8_t GetBannerIndex() const;
};
static_assert(sizeof(RCT12LargeSceneryElement) == 8);
struct RCT12WallElement : RCT12TileElementBase
{
private:
    uint8_t EntryIndex; // 4
    union
    {
        uint8_t Colour3;     // 5
        uint8_t BannerIndex; // 5
    };
    uint8_t Colour1;   // 6 0b_2221_1111 2 = colour_2 (uses Flags for rest of colour2), 1 = colour_1
    uint8_t Animation; // 7 0b_dfff_ft00 d = direction, f = frame num, t = across track flag (not used)
public:
    RCT12ObjectEntryIndex GetEntryIndex() const;
    uint8_t GetSlope() const;
    OpenRCT2::Drawing::Colour GetPrimaryColour() const;
    OpenRCT2::Drawing::Colour GetSecondaryColour() const;
    OpenRCT2::Drawing::Colour GetTertiaryColour() const;
    uint8_t GetAnimationFrame() const;
    uint8_t GetBannerIndex() const;
    bool IsAcrossTrack() const;
    bool AnimationIsBackwards() const;
    int32_t GetRCT1WallType(int32_t edge) const;
    OpenRCT2::Drawing::Colour GetRCT1WallColour() const;
    uint8_t GetRCT1Slope() const;
};
static_assert(sizeof(RCT12WallElement) == 8);
struct RCT12EntranceElement : RCT12TileElementBase
{
private:
    uint8_t EntranceType; // 4
    uint8_t Index;        // 5. 0bUSSS????, S = station index.
    uint8_t PathType;     // 6
    uint8_t RideIndex;    // 7
public:
    uint8_t GetEntranceType() const;
    uint8_t GetRideIndex() const;
    uint8_t GetStationIndex() const;
    uint8_t GetSequenceIndex() const;
    uint8_t GetPathType() const;
};
static_assert(sizeof(RCT12EntranceElement) == 8);
struct RCT12BannerElement : RCT12TileElementBase
{
private:
    uint8_t Index;        // 4
    uint8_t Position;     // 5
    uint8_t AllowedEdges; // 6
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t Unused; // 7
#pragma clang diagnostic pop
public:
    uint8_t GetIndex() const;
    uint8_t GetPosition() const;
    uint8_t GetAllowedEdges() const;
};
static_assert(sizeof(RCT12BannerElement) == 8);

struct RCT12CorruptElement : RCT12TileElementBase
{
    uint8_t Pad[4];
};
static_assert(sizeof(RCT12CorruptElement) == 8);

struct RCT12EightCarsCorruptElement14 : RCT12TileElementBase
{
    uint8_t Pad[4];
};
static_assert(sizeof(RCT12EightCarsCorruptElement14) == 8);

struct RCT12EightCarsCorruptElement15 : RCT12TileElementBase
{
    uint8_t Pad[4];
};
static_assert(sizeof(RCT12EightCarsCorruptElement15) == 8);

// Offset into EntityListHead and EntityListCount
enum class RCT12EntityLinkListOffset : uint8_t
{
    free = 0,
    trainHead = 1 * sizeof(uint16_t),
    peep = 2 * sizeof(uint16_t),
    misc = 3 * sizeof(uint16_t),
    litter = 4 * sizeof(uint16_t),
    vehicle = 5 * sizeof(uint16_t),
};

enum class RCT12EntityIdentifier : uint8_t
{
    vehicle = 0,
    peep = 1,
    misc = 2,
    litter = 3,
    null = 255
};

enum class RCT12MiscEntityType : uint8_t
{
    steamParticle,
    moneyEffect,
    crashedVehicleParticle,
    explosionCloud,
    crashSplash,
    explosionFlare,
    jumpingFountainWater,
    balloon,
    duck,
    jumpingFountainSnow
};

enum class RCT12PeepType : uint8_t
{
    guest,
    staff,

    invalid = 0xFF
};

enum class RCT12PeepAnimationGroup : uint8_t
{
    normal = 0,
    handyman = 1,
    mechanic = 2,
    security = 3,
    entertainerPanda = 4,
    entertainerTiger = 5,
    entertainerElephant = 6,
    entertainerRoman = 7,
    entertainerGorilla = 8,
    entertainerSnowman = 9,
    entertainerKnight = 10,
    entertainerAstronaut = 11,
    entertainerBandit = 12,
    entertainerSheriff = 13,
    entertainerPirate = 14,
    iceCream = 15,
    chips = 16,
    burger = 17,
    drink = 18,
    balloon = 19,
    candyfloss = 20,
    umbrella = 21,
    pizza = 22,
    securityAlt = 23,
    popcorn = 24,
    armsCrossed = 25,
    headDown = 26,
    nauseous = 27,
    veryNauseous = 28,
    requireToilet = 29,
    hat = 30,
    hotDog = 31,
    tentacle = 32,
    toffeeApple = 33,
    doughnut = 34,
    coffee = 35,
    chicken = 36,
    lemonade = 37,
    watching = 38,
    pretzel = 39,
    sunglasses = 40,
    sujeonggwa = 41,
    juice = 42,
    funnelCake = 43,
    noodles = 44,
    sausage = 45,
    soup = 46,
    sandwich = 47,
    count = 48,

    invalid = 255
};

struct RCT12EntityBase
{
    RCT12EntityIdentifier EntityIdentifier;         // 0x00
    uint8_t Type;                                   // 0x01
    uint16_t NextInQuadrant;                        // 0x02
    uint16_t Next;                                  // 0x04
    uint16_t Previous;                              // 0x06
    RCT12EntityLinkListOffset LinkedListTypeOffset; // 0x08
    uint8_t SpriteHeightNegative;                   // 0x09
    uint16_t EntityIndex;                           // 0x0A
    uint16_t Flags;                                 // 0x0C
    int16_t x;                                      // 0x0E
    int16_t y;                                      // 0x10
    int16_t z;                                      // 0x12
    uint8_t SpriteWidth;                            // 0x14
    uint8_t SpriteHeightPositive;                   // 0x15
    int16_t SpriteLeft;                             // 0x16
    int16_t SpriteTop;                              // 0x18
    int16_t SpriteRight;                            // 0x1A
    int16_t SpriteBottom;                           // 0x1C
    uint8_t EntityDirection;                        // 0x1E
};
static_assert(sizeof(RCT12EntityBase) == 0x1F);

struct RCT12EntityBalloon : RCT12EntityBase
{
    uint8_t Pad1F[0x24 - 0x1F];
    uint16_t Popped;    // 0x24
    uint8_t TimeToMove; // 0x26
    uint8_t Frame;      // 0x27
    uint8_t Pad28[4];
    uint8_t Colour; // 0x2C
};
static_assert(sizeof(RCT12EntityBalloon) == 0x2D);

struct RCT12EntityDuck : RCT12EntityBase
{
    uint8_t Pad1F[0x26 - 0x1F];
    uint16_t Frame; // 0x26
    uint8_t Pad28[0x30 - 0x28];
    int16_t TargetX; // 0x30
    int16_t TargetY; // 0x32
    uint8_t Pad34[0x14];
    uint8_t State; // 0x48
};
static_assert(sizeof(RCT12EntityDuck) == 0x49);

struct RCT12EntityLitter : RCT12EntityBase
{
    uint8_t Pad1F[0x24 - 0x1F];
    uint32_t CreationTick; // 0x24
};
static_assert(sizeof(RCT12EntityLitter) == 0x28);

struct RCT12EntityParticle : RCT12EntityBase
{
    uint8_t Pad1F[0x26 - 0x1F];
    uint16_t Frame; // 0x26
};
static_assert(sizeof(RCT12EntityParticle) == 0x28);

struct RCT12EntityJumpingFountain : RCT12EntityBase
{
    uint8_t Pad1F[0x26 - 0x1F];
    uint8_t NumTicksAlive; // 0x26
    uint8_t Frame;         // 0x27
    uint8_t Pad28[0x2F - 0x28];
    OpenRCT2::FountainFlags fountainFlags{}; // 0x2F
    int16_t TargetX;                         // 0x30
    int16_t TargetY;                         // 0x32
    uint8_t Pad34[0x46 - 0x34];
    uint16_t Iteration; // 0x46
};
static_assert(sizeof(RCT12EntityJumpingFountain) == 0x48);

struct RCT12EntityMoneyEffect : RCT12EntityBase
{
    uint8_t Pad1F[0x24 - 0x1F];
    uint16_t MoveDelay;   // 0x24
    uint8_t NumMovements; // 0x26
    uint8_t Vertical;
    money32 Value; // 0x28
    uint8_t Pad2C[0x44 - 0x2C];
    int16_t OffsetX; // 0x44
    uint16_t Wiggle; // 0x46
};
static_assert(sizeof(RCT12EntityMoneyEffect) == 0x48);

struct RCT12EntityCrashedVehicleParticle : RCT12EntityBase
{
    uint8_t Pad1F[0x24 - 0x1F];
    uint16_t TimeToLive; // 0x24
    uint16_t Frame;      // 0x26
    uint8_t Pad28[0x2C - 0x28];
    uint8_t Colour[2];          // 0x2C
    uint16_t CrashedEntityBase; // 0x2E
    int16_t VelocityX;          // 0x30
    int16_t VelocityY;          // 0x32
    int16_t VelocityZ;          // 0x34
    uint8_t Pad36[0x38 - 0x36];
    int32_t AccelerationX; // 0x38
    int32_t AccelerationY; // 0x3C
    int32_t AccelerationZ; // 0x40
};
static_assert(sizeof(RCT12EntityCrashedVehicleParticle) == 0x44);

struct RCT12EntityCrashSplash : RCT12EntityBase
{
    uint8_t Pad1F[0x26 - 0x1F];
    uint16_t Frame; // 0x26
};
static_assert(sizeof(RCT12EntityCrashSplash) == 0x28);

struct RCT12EntitySteamParticle : RCT12EntityBase
{
    uint8_t Pad1F[0x24 - 0x1F];
    uint16_t TimeToMove; // 0x24
    uint16_t Frame;      // 0x26
};
static_assert(sizeof(RCT12EntitySteamParticle) == 0x28);

struct RCT12PeepThought
{
    uint8_t Type;
    uint8_t Item;
    uint8_t Freshness;
    uint8_t FreshTimeout;
};
static_assert(sizeof(RCT12PeepThought) == 4);

struct RCT12RideMeasurement
{
    uint8_t RideIndex;                                                   // 0x0000
    uint8_t Flags;                                                       // 0x0001
    uint32_t LastUseTick;                                                // 0x0002
    uint16_t NumItems;                                                   // 0x0006
    uint16_t CurrentItem;                                                // 0x0008
    uint8_t VehicleIndex;                                                // 0x000A
    uint8_t CurrentStation;                                              // 0x000B
    int8_t Vertical[OpenRCT2::RCT12::Limits::kRideMeasurementMaxItems];  // 0x000C
    int8_t Lateral[OpenRCT2::RCT12::Limits::kRideMeasurementMaxItems];   // 0x12CC
    uint8_t Velocity[OpenRCT2::RCT12::Limits::kRideMeasurementMaxItems]; // 0x258C
    uint8_t Altitude[OpenRCT2::RCT12::Limits::kRideMeasurementMaxItems]; // 0x384C
};
static_assert(sizeof(RCT12RideMeasurement) == 0x4B0C);

struct RCT12Banner
{
    RCT12ObjectEntryIndex Type;
    BannerFlags flags;   // 0x01
    ::StringId StringID; // 0x02
    union
    {
        uint8_t Colour;    // 0x04
        uint8_t RideIndex; // 0x04
    };
    OpenRCT2::Drawing::TextColour textColour; // 0x05
    uint8_t x;                                // 0x06
    uint8_t y;                                // 0x07
};
static_assert(sizeof(RCT12Banner) == 8);

struct RCT12MapAnimation
{
    uint8_t BaseZ;
    uint8_t Type;
    uint16_t x;
    uint16_t y;
};
static_assert(sizeof(RCT12MapAnimation) == 6);

static constexpr uint8_t kRCT12MapAnimationTypeOnRidePhoto = 6;
static constexpr uint8_t kRCT12MapAnimationTypeLandEdgeDoor = 9;
static constexpr uint8_t kRCT12MapAnimationTypeWallDoor = 12;

struct RCT12ResearchItem
{
    // Bit 16 (0: scenery entry, 1: ride entry)
    union
    {
        uint32_t RawValue;
        struct
        {
            RCT12ObjectEntryIndex EntryIndex;
            uint8_t BaseRideType;
            uint8_t Type; // 0: scenery entry, 1: ride entry
            uint8_t Flags;
        };
    };
    uint8_t Category;

    bool IsInventedEndMarker() const;
    bool IsRandomEndMarker() const;
    bool IsUninventedEndMarker() const;

    ResearchItem ToResearchItem() const;
};
static_assert(sizeof(RCT12ResearchItem) == 5);

struct RCT12VehicleColour
{
    uint8_t BodyColour;
    uint8_t TrimColour;
};
static_assert(sizeof(RCT12VehicleColour) == 2);

#pragma pack(pop)

OpenRCT2::ObjectEntryIndex RCTEntryIndexToOpenRCT2EntryIndex(RCT12ObjectEntryIndex index);
RideId RCT12RideIdToOpenRCT2RideId(RCT12RideId rideId);
bool IsLikelyUTF8(std::string_view s);
std::string RCT12RemoveFormattingUTF8(std::string_view s);
std::string ConvertFormattedStringToOpenRCT2(std::string_view buffer);
OpenRCT2::TrackElemType RCT12FlatTrackTypeToOpenRCT2(OpenRCT2::RCT12::TrackElemType origTrackType);
OpenRCT2::RCT12::TrackElemType OpenRCT2FlatTrackTypeToRCT12(OpenRCT2::TrackElemType origTrackType);
std::string_view GetStationIdentifierFromStyle(uint8_t style);
uint8_t GetStationStyleFromIdentifier(u8string_view identifier);
std::optional<uint8_t> GetStyleFromMusicIdentifier(std::string_view identifier);
void RCT12AddDefaultMusic(OpenRCT2::ObjectList& objectList);
void AppendRequiredObjects(
    OpenRCT2::ObjectList& objectList, OpenRCT2::ObjectType objectType, std::span<const std::string_view> objectNames);
void AppendRequiredObjects(
    OpenRCT2::ObjectList& objectList, OpenRCT2::ObjectType objectType, const OpenRCT2::RCT12::EntryList& entryList);
bool IsUserStringID(StringId stringId);

static constexpr money32 kRCT12CompanyValueOnFailedObjective = 0x80000001;

money64 RCT12CompletedCompanyValueToOpenRCT2(money32 origValue);

template<typename T>
std::vector<uint16_t> RCT12GetRideTypesBeenOn(T* srcPeep)
{
    std::vector<uint16_t> ridesTypesBeenOn;
    for (uint16_t i = 0; i < OpenRCT2::RCT12::Limits::kMaxRideObjects; i++)
    {
        if (srcPeep->RideTypesBeenOn[i / 8] & (1 << (i % 8)))
        {
            ridesTypesBeenOn.push_back(i);
        }
    }
    return ridesTypesBeenOn;
}
template<typename T>
std::vector<RideId> RCT12GetRidesBeenOn(T* srcPeep)
{
    std::vector<RideId> ridesBeenOn;
    for (uint16_t i = 0; i < OpenRCT2::RCT12::Limits::kMaxRidesInPark; i++)
    {
        if (srcPeep->RidesBeenOn[i / 8] & (1 << (i % 8)))
        {
            ridesBeenOn.push_back(RideId::FromUnderlying(i));
        }
    }
    return ridesBeenOn;
}

namespace OpenRCT2::RCT12
{
    /**
     * Iterates an RCT string buffer and returns the length of the string in bytes.
     * Handles single and multi-byte strings.
     */
    size_t GetRCTStringBufferLen(const char* buffer, size_t maxBufferLen);
    bool TrackTypeHasSpeedSetting(TrackElemType trackType);
    bool TrackTypeIsStation(TrackElemType trackType);
} // namespace OpenRCT2::RCT12
