/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

// Structures shared between both RCT1 and RCT2.

#include "../common.h"
#include "../management/Research.h"
#include "../object/Object.h"
#include "../ride/RideTypes.h"
#include "../world/tile_element/TileElementType.h"
#include "Limits.h"

#include <string>
#include <string_view>
#include <vector>

class ObjectList;

using track_type_t = uint16_t;
using RCT12TrackType = uint8_t;
namespace RCT12
{
    class EntryList;
}

constexpr uint8_t RCT2_STRING_FORMAT_ARG_START = 123;
constexpr uint8_t RCT2_STRING_FORMAT_ARG_END = 141;
constexpr uint8_t RCT2_STRING_FORMAT_COLOUR_START = 142;
constexpr uint8_t RCT2_STRING_FORMAT_COLOUR_END = 156;

constexpr const uint8_t RCT12_SOUND_ID_NULL = 0xFF;

using RCT12RideId = uint8_t;
constexpr const RCT12RideId RCT12_RIDE_ID_NULL = 255;

constexpr uint8_t RCT12_BANNER_INDEX_NULL = std::numeric_limits<uint8_t>::max();

constexpr const uint8_t RCT12_TILE_ELEMENT_SURFACE_EDGE_STYLE_MASK = 0xE0;   // in RCT12TileElement.properties.surface.slope
constexpr const uint8_t RCT12_TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK = 0x1F; // in RCT12TileElement.properties.surface.terrain
constexpr const uint8_t RCT12_TILE_ELEMENT_SURFACE_TERRAIN_MASK = 0xE0;      // in RCT12TileElement.properties.surface.terrain

constexpr const uint8_t RCT12_SMALL_SCENERY_ELEMENT_NEEDS_SUPPORTS_FLAG = 0x20;
constexpr const uint8_t RCT12_TILE_ELEMENT_COLOUR_MASK = 0b0001'1111;

constexpr const uint16_t RCT12_TILE_ELEMENT_LARGE_TYPE_MASK = 0x3FF;

constexpr uint16_t const RCT12_XY8_UNDEFINED = 0xFFFF;

using RCT12ObjectEntryIndex = uint8_t;
constexpr const RCT12ObjectEntryIndex RCT12_OBJECT_ENTRY_INDEX_NULL = 255;

// Everything before this point has been researched
constexpr const uint32_t RCT12_RESEARCHED_ITEMS_SEPARATOR = 0xFFFFFFFF;
// Everything before this point and after separator still requires research
constexpr const uint32_t RCT12_RESEARCHED_ITEMS_END = 0xFFFFFFFE;
// Extra end of list entry. Leftover from RCT1.
constexpr const uint32_t RCT12_RESEARCHED_ITEMS_END_2 = 0xFFFFFFFD;

constexpr const uint16_t RCT12_PEEP_SPAWN_UNDEFINED = 0xFFFF;

constexpr const uint16_t RCT12VehicleTrackDirectionMask = 0b0000000000000011;
constexpr const uint16_t RCT12VehicleTrackTypeMask = 0b1111111111111100;

constexpr const uint8_t RCT12PeepThoughtItemNone = std::numeric_limits<uint8_t>::max();

constexpr const uint8_t RCT12GuestsInParkHistoryFactor = 20;
constexpr const uint8_t RCT12ParkHistoryUndefined = std::numeric_limits<uint8_t>::max();

enum class RCT12TrackDesignVersion : uint8_t
{
    TD4,
    TD4_AA,
    TD6,
    unknown
};

enum
{
    RCT12_SURFACE_ELEMENT_TYPE_SURFACE_MASK = 0b00000011,
    RCT12_SURFACE_ELEMENT_TYPE_EDGE_MASK = 0b01000000,
};

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
    RCT12_TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT = 1 << 7,
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
        return xy == RCT12_XY8_UNDEFINED;
    }

    void SetNull()
    {
        xy = RCT12_XY8_UNDEFINED;
    }
};
assert_struct_size(RCT12xy8, 2);

/* Maze Element entry   size: 0x04 */
struct TD46MazeElement
{
    union
    {
        uint32_t All;
        struct
        {
            int8_t x;
            int8_t y;
            union
            {
                uint16_t MazeEntry;
                struct
                {
                    uint8_t Direction;
                    uint8_t Type;
                };
            };
        };
    };
};
assert_struct_size(TD46MazeElement, 0x04);

/* Track Element entry  size: 0x02 */
struct TD46TrackElement
{
    uint8_t Type;  // 0x00
    uint8_t Flags; // 0x01
};
assert_struct_size(TD46TrackElement, 0x02);

struct RCT12Award
{
    uint16_t Time;
    uint16_t Type;
};
assert_struct_size(RCT12Award, 4);

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
assert_struct_size(RCT12NewsItem, 0x10C);

struct RCT12xyzd8
{
    uint8_t x, y, z, direction;
};
assert_struct_size(RCT12xyzd8, 4);

struct RCT12PeepSpawn
{
    uint16_t x;
    uint16_t y;
    uint8_t z;
    uint8_t direction;
};
assert_struct_size(RCT12PeepSpawn, 6);

enum class RCT12TileElementType : uint8_t
{
    Surface = 0,
    Path = 1,
    Track = 2,
    SmallScenery = 3,
    Entrance = 4,
    Wall = 5,
    LargeScenery = 6,
    Banner = 7,
    Corrupt = 8,
    EightCarsCorrupt14 = 14,
    EightCarsCorrupt15 = 15,
};

constexpr TileElementType ToOpenRCT2TileElementType(RCT12TileElementType rct12type)
{
    switch (rct12type)
    {
        case RCT12TileElementType::Surface:
        case RCT12TileElementType::Path:
        case RCT12TileElementType::Track:
        case RCT12TileElementType::SmallScenery:
        case RCT12TileElementType::Entrance:
        case RCT12TileElementType::Wall:
        case RCT12TileElementType::LargeScenery:
        case RCT12TileElementType::Banner:
            return static_cast<TileElementType>(rct12type);

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
    template<typename TType, RCT12TileElementType TClass> const TType* as() const
    {
        return static_cast<RCT12TileElementType>(GetType()) == TClass ? reinterpret_cast<const TType*>(this) : nullptr;
    }
    template<typename TType, RCT12TileElementType TClass> TType* as()
    {
        return static_cast<RCT12TileElementType>(GetType()) == TClass ? reinterpret_cast<TType*>(this) : nullptr;
    }

    const RCT12SurfaceElement* AsSurface() const
    {
        return as<RCT12SurfaceElement, RCT12TileElementType::Surface>();
    }
    RCT12SurfaceElement* AsSurface()
    {
        return as<RCT12SurfaceElement, RCT12TileElementType::Surface>();
    }
    const RCT12PathElement* AsPath() const
    {
        return as<RCT12PathElement, RCT12TileElementType::Path>();
    }
    RCT12PathElement* AsPath()
    {
        return as<RCT12PathElement, RCT12TileElementType::Path>();
    }
    const RCT12TrackElement* AsTrack() const
    {
        return as<RCT12TrackElement, RCT12TileElementType::Track>();
    }
    RCT12TrackElement* AsTrack()
    {
        return as<RCT12TrackElement, RCT12TileElementType::Track>();
    }
    const RCT12SmallSceneryElement* AsSmallScenery() const
    {
        return as<RCT12SmallSceneryElement, RCT12TileElementType::SmallScenery>();
    }
    RCT12SmallSceneryElement* AsSmallScenery()
    {
        return as<RCT12SmallSceneryElement, RCT12TileElementType::SmallScenery>();
    }
    const RCT12LargeSceneryElement* AsLargeScenery() const
    {
        return as<RCT12LargeSceneryElement, RCT12TileElementType::LargeScenery>();
    }
    RCT12LargeSceneryElement* AsLargeScenery()
    {
        return as<RCT12LargeSceneryElement, RCT12TileElementType::LargeScenery>();
    }
    const RCT12WallElement* AsWall() const
    {
        return as<RCT12WallElement, RCT12TileElementType::Wall>();
    }
    RCT12WallElement* AsWall()
    {
        return as<RCT12WallElement, RCT12TileElementType::Wall>();
    }
    const RCT12EntranceElement* AsEntrance() const
    {
        return as<RCT12EntranceElement, RCT12TileElementType::Entrance>();
    }
    RCT12EntranceElement* AsEntrance()
    {
        return as<RCT12EntranceElement, RCT12TileElementType::Entrance>();
    }
    const RCT12BannerElement* AsBanner() const
    {
        return as<RCT12BannerElement, RCT12TileElementType::Banner>();
    }
    RCT12BannerElement* AsBanner()
    {
        return as<RCT12BannerElement, RCT12TileElementType::Banner>();
    }
};
assert_struct_size(RCT12TileElement, 8);
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
assert_struct_size(RCT12SurfaceElement, 8);
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
assert_struct_size(RCT12PathElement, 8);
struct RCT12TrackElement : RCT12TileElementBase
{
private:
    uint8_t TrackType; // 4
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
    uint8_t GetTrackType() const;
    uint8_t GetSequenceIndex() const;
    uint8_t GetRideIndex() const;
    uint8_t GetColourScheme() const;
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
assert_struct_size(RCT12TrackElement, 8);
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
    colour_t GetPrimaryColour() const;
    colour_t GetSecondaryColour() const;
    bool NeedsSupports() const;
};
assert_struct_size(RCT12SmallSceneryElement, 8);
struct RCT12LargeSceneryElement : RCT12TileElementBase
{
private:
    uint16_t EntryIndex; // 4
    uint8_t Colour[2];   // 6
public:
    uint32_t GetEntryIndex() const;
    uint16_t GetSequenceIndex() const;
    colour_t GetPrimaryColour() const;
    colour_t GetSecondaryColour() const;
    uint8_t GetBannerIndex() const;
};
assert_struct_size(RCT12LargeSceneryElement, 8);
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
    colour_t GetPrimaryColour() const;
    colour_t GetSecondaryColour() const;
    colour_t GetTertiaryColour() const;
    uint8_t GetAnimationFrame() const;
    uint8_t GetBannerIndex() const;
    bool IsAcrossTrack() const;
    bool AnimationIsBackwards() const;
    int32_t GetRCT1WallType(int32_t edge) const;
    colour_t GetRCT1WallColour() const;
    uint8_t GetRCT1Slope() const;
};
assert_struct_size(RCT12WallElement, 8);
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
assert_struct_size(RCT12EntranceElement, 8);
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
assert_struct_size(RCT12BannerElement, 8);

struct RCT12CorruptElement : RCT12TileElementBase
{
    uint8_t Pad[4];
};
assert_struct_size(RCT12CorruptElement, 8);

struct RCT12EightCarsCorruptElement14 : RCT12TileElementBase
{
    uint8_t Pad[4];
};
assert_struct_size(RCT12EightCarsCorruptElement14, 8);

struct RCT12EightCarsCorruptElement15 : RCT12TileElementBase
{
    uint8_t Pad[4];
};
assert_struct_size(RCT12EightCarsCorruptElement15, 8);

// Offset into EntityListHead and EntityListCount
enum class RCT12EntityLinkListOffset : uint8_t
{
    Free = 0,
    TrainHead = 1 * sizeof(uint16_t),
    Peep = 2 * sizeof(uint16_t),
    Misc = 3 * sizeof(uint16_t),
    Litter = 4 * sizeof(uint16_t),
    Vehicle = 5 * sizeof(uint16_t),
};

enum class RCT12EntityIdentifier : uint8_t
{
    Vehicle = 0,
    Peep = 1,
    Misc = 2,
    Litter = 3,
    Null = 255
};

enum class RCT12MiscEntityType : uint8_t
{
    SteamParticle,
    MoneyEffect,
    CrashedVehicleParticle,
    ExplosionCloud,
    CrashSplash,
    ExplosionFlare,
    JumpingFountainWater,
    Balloon,
    Duck,
    JumpingFountainSnow
};

enum class RCT12PeepType : uint8_t
{
    Guest,
    Staff,

    Invalid = 0xFF
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
assert_struct_size(RCT12EntityBase, 0x1F);

struct RCT12EntityBalloon : RCT12EntityBase
{
    uint8_t Pad1F[0x24 - 0x1F];
    uint16_t Popped;    // 0x24
    uint8_t TimeToMove; // 0x26
    uint8_t Frame;      // 0x27
    uint8_t Pad28[4];
    uint8_t Colour; // 0x2C
};
assert_struct_size(RCT12EntityBalloon, 0x2D);

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
assert_struct_size(RCT12EntityDuck, 0x49);

struct RCT12EntityLitter : RCT12EntityBase
{
    uint8_t Pad1F[0x24 - 0x1F];
    uint32_t CreationTick; // 0x24
};
assert_struct_size(RCT12EntityLitter, 0x28);

struct RCT12EntityParticle : RCT12EntityBase
{
    uint8_t Pad1F[0x26 - 0x1F];
    uint16_t Frame; // 0x26
};
assert_struct_size(RCT12EntityParticle, 0x28);

struct RCT12EntityJumpingFountain : RCT12EntityBase
{
    uint8_t Pad1F[0x26 - 0x1F];
    uint8_t NumTicksAlive; // 0x26
    uint8_t Frame;         // 0x27
    uint8_t Pad28[0x2F - 0x28];
    uint8_t FountainFlags; // 0x2F
    int16_t TargetX;       // 0x30
    int16_t TargetY;       // 0x32
    uint8_t Pad34[0x46 - 0x34];
    uint16_t Iteration; // 0x46
};
assert_struct_size(RCT12EntityJumpingFountain, 0x48);

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
assert_struct_size(RCT12EntityMoneyEffect, 0x48);

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
assert_struct_size(RCT12EntityCrashedVehicleParticle, 0x44);

struct RCT12EntityCrashSplash : RCT12EntityBase
{
    uint8_t Pad1F[0x26 - 0x1F];
    uint16_t Frame; // 0x26
};
assert_struct_size(RCT12EntityCrashSplash, 0x28);

struct RCT12EntitySteamParticle : RCT12EntityBase
{
    uint8_t Pad1F[0x24 - 0x1F];
    uint16_t TimeToMove; // 0x24
    uint16_t Frame;      // 0x26
};
assert_struct_size(RCT12EntitySteamParticle, 0x28);

struct RCT12PeepThought
{
    uint8_t Type;
    uint8_t Item;
    uint8_t Freshness;
    uint8_t FreshTimeout;
};
assert_struct_size(RCT12PeepThought, 4);

struct RCT12RideMeasurement
{
    uint8_t RideIndex;                                        // 0x0000
    uint8_t Flags;                                            // 0x0001
    uint32_t LastUseTick;                                     // 0x0002
    uint16_t NumItems;                                        // 0x0006
    uint16_t CurrentItem;                                     // 0x0008
    uint8_t VehicleIndex;                                     // 0x000A
    uint8_t CurrentStation;                                   // 0x000B
    int8_t Vertical[RCT12::Limits::RideMeasurementMaxItems];  // 0x000C
    int8_t Lateral[RCT12::Limits::RideMeasurementMaxItems];   // 0x12CC
    uint8_t Velocity[RCT12::Limits::RideMeasurementMaxItems]; // 0x258C
    uint8_t Altitude[RCT12::Limits::RideMeasurementMaxItems]; // 0x384C
};
assert_struct_size(RCT12RideMeasurement, 0x4B0C);

struct RCT12Banner
{
    RCT12ObjectEntryIndex Type;
    uint8_t Flags;       // 0x01
    ::StringId StringID; // 0x02
    union
    {
        uint8_t Colour;    // 0x04
        uint8_t RideIndex; // 0x04
    };
    uint8_t TextColour; // 0x05
    uint8_t x;          // 0x06
    uint8_t y;          // 0x07
};
assert_struct_size(RCT12Banner, 8);

struct RCT12MapAnimation
{
    uint8_t BaseZ;
    uint8_t Type;
    uint16_t x;
    uint16_t y;
};
assert_struct_size(RCT12MapAnimation, 6);

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
assert_struct_size(RCT12ResearchItem, 5);

struct RCT12VehicleColour
{
    uint8_t BodyColour;
    uint8_t TrimColour;
};
assert_struct_size(RCT12VehicleColour, 2);

#pragma pack(pop)

ObjectEntryIndex RCTEntryIndexToOpenRCT2EntryIndex(const RCT12ObjectEntryIndex index);
RideId RCT12RideIdToOpenRCT2RideId(const RCT12RideId rideId);
bool IsLikelyUTF8(std::string_view s);
std::string RCT12RemoveFormattingUTF8(std::string_view s);
std::string ConvertFormattedStringToOpenRCT2(std::string_view buffer);
track_type_t RCT12FlatTrackTypeToOpenRCT2(RCT12TrackType origTrackType);
RCT12TrackType OpenRCT2FlatTrackTypeToRCT12(track_type_t origTrackType);
std::string_view GetStationIdentifierFromStyle(uint8_t style);
uint8_t GetStationStyleFromIdentifier(u8string_view identifier);
std::optional<uint8_t> GetStyleFromMusicIdentifier(std::string_view identifier);
void RCT12AddDefaultObjects(ObjectList& objectList);
void AppendRequiredObjects(ObjectList& objectList, ObjectType objectType, const RCT12::EntryList& entryList);

static constexpr money32 RCT12_COMPANY_VALUE_ON_FAILED_OBJECTIVE = 0x80000001;

money64 RCT12CompletedCompanyValueToOpenRCT2(money32 origValue);

template<typename T> std::vector<uint16_t> RCT12GetRideTypesBeenOn(T* srcPeep)
{
    std::vector<uint16_t> ridesTypesBeenOn;
    for (uint16_t i = 0; i < RCT12::Limits::MaxRideObjects; i++)
    {
        if (srcPeep->RideTypesBeenOn[i / 8] & (1 << (i % 8)))
        {
            ridesTypesBeenOn.push_back(i);
        }
    }
    return ridesTypesBeenOn;
}
template<typename T> std::vector<RideId> RCT12GetRidesBeenOn(T* srcPeep)
{
    std::vector<RideId> ridesBeenOn;
    for (uint16_t i = 0; i < RCT12::Limits::MaxRidesInPark; i++)
    {
        if (srcPeep->RidesBeenOn[i / 8] & (1 << (i % 8)))
        {
            ridesBeenOn.push_back(RideId::FromUnderlying(i));
        }
    }
    return ridesBeenOn;
}
