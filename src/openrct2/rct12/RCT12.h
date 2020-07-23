/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

// Structures shared between both RCT1 and RCT2.

#include "../common.h"
#include "../object/Object.h"

#include <string>
#include <string_view>

constexpr const uint8_t RCT12_MAX_RIDES_IN_PARK = 255;
constexpr const uint8_t RCT12_MAX_AWARDS = 4;
constexpr const uint8_t RCT12_MAX_NEWS_ITEMS = 61;
constexpr const uint8_t RCT12_MAX_STATIONS_PER_RIDE = 4;
constexpr const uint8_t RCT12_MAX_PEEP_SPAWNS = 2;
constexpr const uint8_t RCT12_MAX_PARK_ENTRANCES = 4;
// The number of elements in the patrol_areas array per staff member. Every bit in the array represents a 4x4 square.
// In RCT1, that's an 8-bit array. 8 * 128 = 1024 bits, which is also the number of 4x4 squares on a 128x128 map.
// For RCT2, it's a 32-bit array. 32 * 128 = 4096 bits, which is also the number of 4x4 squares on a 256x256 map.
constexpr const uint8_t RCT12_PATROL_AREA_SIZE = 128;
constexpr const uint8_t RCT12_STAFF_TYPE_COUNT = 4;
constexpr const uint8_t RCT12_NUM_COLOUR_SCHEMES = 4;
constexpr const uint8_t RCT12_MAX_VEHICLE_COLOURS = 32;
constexpr const uint8_t RCT12_SOUND_ID_NULL = 0xFF;

constexpr const uint8_t RCT12_EXPENDITURE_TABLE_MONTH_COUNT = 16;
constexpr const uint8_t RCT12_EXPENDITURE_TYPE_COUNT = 14;
constexpr const uint8_t RCT12_FINANCE_GRAPH_SIZE = 128;

constexpr const uint16_t RCT12_MAX_USER_STRINGS = 1024;
constexpr const uint8_t RCT12_USER_STRING_MAX_LENGTH = 32;

constexpr const uint8_t RCT12_PEEP_MAX_THOUGHTS = 5;

constexpr const uint8_t RCT12_RIDE_ID_NULL = 255;
constexpr const uint16_t RCT12_RIDE_MEASUREMENT_MAX_ITEMS = 4800;

constexpr uint16_t const RCT12_MAX_INVERSIONS = 31;
constexpr uint16_t const RCT12_MAX_GOLF_HOLES = 31;
constexpr uint16_t const RCT12_MAX_HELICES = 31;

constexpr uint8_t RCT12_BANNER_INDEX_NULL = std::numeric_limits<uint8_t>::max();

constexpr const uint8_t RCT12_TILE_ELEMENT_SURFACE_EDGE_STYLE_MASK = 0xE0;   // in RCT12TileElement.properties.surface.slope
constexpr const uint8_t RCT12_TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK = 0x1F; // in RCT12TileElement.properties.surface.terrain
constexpr const uint8_t RCT12_TILE_ELEMENT_SURFACE_TERRAIN_MASK = 0xE0;      // in RCT12TileElement.properties.surface.terrain

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

constexpr const uint8_t RCT12_MAX_ELEMENT_HEIGHT = 255;

constexpr const uint16_t RCT12_PEEP_SPAWN_UNDEFINED = 0xFFFF;

enum class RCT12TrackDesignVersion : uint8_t
{
    TD4,
    TD4_AA,
    TD6,
    unknown
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

// Masks and flags for values stored in TileElement.properties.path.type
enum
{
    RCT12_FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK = (1 << 0) | (1 << 1),
    RCT12_FOOTPATH_PROPERTIES_FLAG_IS_SLOPED = (1 << 2),
    RCT12_FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER = (1 << 3),
    RCT12_FOOTPATH_PROPERTIES_TYPE_MASK = (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7),
};

// Masks and flags for values stored in in RCT12TileElement.properties.path.additions
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

    bool isNull() const
    {
        return xy == RCT12_XY8_UNDEFINED;
    }

    void setNull()
    {
        xy = RCT12_XY8_UNDEFINED;
    }
};
assert_struct_size(RCT12xy8, 2);

/* Maze Element entry   size: 0x04 */
struct rct_td46_maze_element
{
    union
    {
        uint32_t all;
        struct
        {
            int8_t x;
            int8_t y;
            union
            {
                uint16_t maze_entry;
                struct
                {
                    uint8_t direction;
                    uint8_t type;
                };
            };
        };
    };
};
assert_struct_size(rct_td46_maze_element, 0x04);

/* Track Element entry  size: 0x02 */
struct rct_td46_track_element
{
    uint8_t type;  // 0x00
    uint8_t flags; // 0x01
};
assert_struct_size(rct_td46_track_element, 0x02);

struct rct12_award
{
    uint16_t time;
    uint16_t type;
};
assert_struct_size(rct12_award, 4);

/**
 * A single news item / message.
 * size: 0x10C
 */
struct rct12_news_item
{
    uint8_t Type;
    uint8_t Flags;
    uint32_t Assoc;
    uint16_t Ticks;
    uint16_t MonthYear;
    uint8_t Day;
    uint8_t pad_0B;
    char Text[256];
};
assert_struct_size(rct12_news_item, 0x10C);

struct rct12_xyzd8
{
    uint8_t x, y, z, direction;
};
assert_struct_size(rct12_xyzd8, 4);

struct rct12_peep_spawn
{
    uint16_t x;
    uint16_t y;
    uint8_t z;
    uint8_t direction;
};
assert_struct_size(rct12_peep_spawn, 6);

enum class RCT12TileElementType : uint8_t
{
    Surface = (0 << 2),
    Path = (1 << 2),
    Track = (2 << 2),
    SmallScenery = (3 << 2),
    Entrance = (4 << 2),
    Wall = (5 << 2),
    LargeScenery = (6 << 2),
    Banner = (7 << 2),
    Corrupt = (8 << 2),
    EightCarsCorrupt14 = (14 << 2),
    EightCarsCorrupt15 = (15 << 2),
};
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
    uint8_t type;             // 0
    uint8_t flags;            // 1. Upper nibble: flags. Lower nibble: occupied quadrants (one bit per quadrant).
    uint8_t base_height;      // 2
    uint8_t clearance_height; // 3
    uint8_t GetType() const;
    uint8_t GetDirection() const;
    void SetDirection(uint8_t direction);

    uint8_t GetOccupiedQuadrants() const;
    void SetOccupiedQuadrants(uint8_t quadrants);

    bool IsLastForTile() const;
    void SetLastForTile(bool on);
    bool IsGhost() const;
    void SetGhost(bool isGhost);
};
/**
 * Map element structure
 * size: 0x08
 */
struct RCT12TileElement : public RCT12TileElementBase
{
    uint8_t pad_04[4];
    template<typename TType, RCT12TileElementType TClass> TType* as() const
    {
        return static_cast<RCT12TileElementType>(GetType()) == TClass
            ? reinterpret_cast<TType*>(const_cast<RCT12TileElement*>(this))
            : nullptr;
    }

    RCT12SurfaceElement* AsSurface() const
    {
        return as<RCT12SurfaceElement, RCT12TileElementType::Surface>();
    }
    RCT12PathElement* AsPath() const
    {
        return as<RCT12PathElement, RCT12TileElementType::Path>();
    }
    RCT12TrackElement* AsTrack() const
    {
        return as<RCT12TrackElement, RCT12TileElementType::Track>();
    }
    RCT12SmallSceneryElement* AsSmallScenery() const
    {
        return as<RCT12SmallSceneryElement, RCT12TileElementType::SmallScenery>();
    }
    RCT12LargeSceneryElement* AsLargeScenery() const
    {
        return as<RCT12LargeSceneryElement, RCT12TileElementType::LargeScenery>();
    }
    RCT12WallElement* AsWall() const
    {
        return as<RCT12WallElement, RCT12TileElementType::Wall>();
    }
    RCT12EntranceElement* AsEntrance() const
    {
        return as<RCT12EntranceElement, RCT12TileElementType::Entrance>();
    }
    RCT12BannerElement* AsBanner() const
    {
        return as<RCT12BannerElement, RCT12TileElementType::Banner>();
    }
    void ClearAs(uint8_t newType);
    uint8_t GetBannerIndex();
};
assert_struct_size(RCT12TileElement, 8);
struct RCT12SurfaceElement : RCT12TileElementBase
{
private:
    uint8_t slope;        // 4 0xE0 Edge Style, 0x1F Slope
    uint8_t terrain;      // 5 0xE0 Terrain Style, 0x1F Water height
    uint8_t grass_length; // 6
    uint8_t ownership;    // 7
public:
    uint8_t GetSlope() const;
    uint32_t GetSurfaceStyle() const;
    uint32_t GetEdgeStyle() const;
    uint8_t GetGrassLength() const;
    uint8_t GetOwnership() const;
    uint32_t GetWaterHeight() const;
    uint8_t GetParkFences() const;
    bool HasTrackThatNeedsWater() const;

    void SetSlope(uint8_t newSlope);
    void SetSurfaceStyle(uint32_t newStyle);
    void SetEdgeStyle(uint32_t newStyle);
    void SetGrassLength(uint8_t newLength);
    void SetOwnership(uint8_t newOwnership);
    void SetWaterHeight(uint32_t newWaterHeight);
    void SetParkFences(uint8_t newParkFences);
    void SetHasTrackThatNeedsWater(bool on);
};
assert_struct_size(RCT12SurfaceElement, 8);
struct RCT12PathElement : RCT12TileElementBase
{
private:
    uint8_t entryIndex; // 4, 0xF0 Path type, 0x08 Ride sign, 0x04 Set when path is sloped, 0x03 Rotation
    uint8_t additions;  // 5, 0bGSSSAAAA: G = Ghost, S = station index, A = addition (0 means no addition)
    uint8_t edges;      // 6
    union
    {
        uint8_t additionStatus; // 7
        uint8_t rideIndex;
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

    void SetPathEntryIndex(RCT12ObjectEntryIndex newIndex);
    void SetQueueBannerDirection(uint8_t direction);
    void SetSloped(bool isSloped);
    void SetSlopeDirection(uint8_t newSlope);
    void SetRideIndex(uint8_t newRideIndex);
    void SetStationIndex(uint8_t newStationIndex);
    void SetWide(bool isWide);
    void SetIsQueue(bool isQueue);
    void SetHasQueueBanner(bool hasQueueBanner);
    void SetEdges(uint8_t newEdges);
    void SetCorners(uint8_t newCorners);
    void SetAddition(uint8_t newAddition);
    void SetAdditionIsGhost(bool isGhost);
    void SetAdditionStatus(uint8_t newStatus);

    bool IsBroken() const;
    void SetIsBroken(bool isBroken);

    bool IsBlockedByVehicle() const;
    void SetIsBlockedByVehicle(bool isBlocked);
};
assert_struct_size(RCT12PathElement, 8);
struct RCT12TrackElement : RCT12TileElementBase
{
private:
    uint8_t trackType; // 4
    union
    {
        struct
        {
            // The lower 4 bits are the track sequence.
            // The upper 4 bits are either station bits or on-ride photo bits.
            //
            // Station bits:
            // - Bit 8 marks green light
            // - Bit 5-7 are station index.
            //
            // On-ride photo bits:
            // - Bits 7 and 8 are never set
            // - Bits 5 and 6 are set when a vehicle triggers the on-ride photo and act like a countdown from 3.
            // - If any of the bits 5-8 are set, the game counts it as a photo being taken.
            uint8_t sequence; // 5.
            uint8_t colour;   // 6
        };
        uint16_t mazeEntry; // 5
    };
    uint8_t rideIndex; // 7
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
    // Used in RCT1, will be reintroduced at some point.
    // (See https://github.com/OpenRCT2/OpenRCT2/issues/7059)
    uint8_t GetDoorAState() const;
    uint8_t GetDoorBState() const;

    void SetTrackType(uint8_t newEntryIndex);
    void SetSequenceIndex(uint8_t newSequenceIndex);
    void SetRideIndex(uint8_t newRideIndex);
    void SetColourScheme(uint8_t newColourScheme);
    void SetStationIndex(uint8_t newStationIndex);
    void SetHasChain(bool on);
    void SetHasCableLift(bool on);
    void SetInverted(bool inverted);
    bool BlockBrakeClosed() const;
    void SetBlockBrakeClosed(bool isClosed);
    void SetBrakeBoosterSpeed(uint8_t speed);
    void SetHasGreenLight(uint8_t greenLight);
    void SetSeatRotation(uint8_t newSeatRotation);
    void SetMazeEntry(uint16_t newMazeEntry);
    void SetPhotoTimeout(uint8_t newValue);

    bool IsIndestructible() const;
    void SetIsIndestructible(bool isIndestructible);
};
assert_struct_size(RCT12TrackElement, 8);
struct RCT12SmallSceneryElement : RCT12TileElementBase
{
private:
    uint8_t entryIndex; // 4
    uint8_t age;        // 5
    uint8_t colour_1;   // 6
    uint8_t colour_2;   // 7
public:
    RCT12ObjectEntryIndex GetEntryIndex() const;
    uint8_t GetAge() const;
    uint8_t GetSceneryQuadrant() const;
    colour_t GetPrimaryColour() const;
    colour_t GetSecondaryColour() const;
    bool NeedsSupports() const;

    void SetEntryIndex(RCT12ObjectEntryIndex newIndex);
    void SetAge(uint8_t newAge);
    void SetSceneryQuadrant(uint8_t newQuadrant);
    void SetPrimaryColour(colour_t colour);
    void SetSecondaryColour(colour_t colour);
    void SetNeedsSupports();
};
assert_struct_size(RCT12SmallSceneryElement, 8);
struct RCT12LargeSceneryElement : RCT12TileElementBase
{
private:
    uint16_t entryIndex; // 4
    uint8_t colour[2];   // 6
public:
    uint32_t GetEntryIndex() const;
    uint16_t GetSequenceIndex() const;
    colour_t GetPrimaryColour() const;
    colour_t GetSecondaryColour() const;
    uint8_t GetBannerIndex() const;

    void SetEntryIndex(uint32_t newIndex);
    void SetSequenceIndex(uint16_t sequence);
    void SetPrimaryColour(colour_t colour);
    void SetSecondaryColour(colour_t colour);
    void SetBannerIndex(uint8_t newIndex);
};
assert_struct_size(RCT12LargeSceneryElement, 8);
struct RCT12WallElement : RCT12TileElementBase
{
private:
    uint8_t entryIndex; // 4
    union
    {
        uint8_t colour_3;     // 5
        uint8_t banner_index; // 5
    };
    uint8_t colour_1;  // 6 0b_2221_1111 2 = colour_2 (uses flags for rest of colour2), 1 = colour_1
    uint8_t animation; // 7 0b_dfff_ft00 d = direction, f = frame num, t = across track flag (not used)
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
    uint32_t GetRawRCT1WallTypeData() const;
    int32_t GetRCT1WallType(int32_t edge) const;
    colour_t GetRCT1WallColour() const;

    void SetEntryIndex(RCT12ObjectEntryIndex newIndex);
    void SetSlope(uint8_t newslope);
    void SetPrimaryColour(colour_t newColour);
    void SetSecondaryColour(colour_t newColour);
    void SetTertiaryColour(colour_t newColour);
    void SetAnimationFrame(uint8_t frameNum);
    void SetBannerIndex(uint8_t newIndex);
    void SetAcrossTrack(bool acrossTrack);
    void SetAnimationIsBackwards(bool isBackwards);
};
assert_struct_size(RCT12WallElement, 8);
struct RCT12EntranceElement : RCT12TileElementBase
{
private:
    uint8_t entranceType; // 4
    uint8_t index;        // 5. 0bUSSS????, S = station index.
    uint8_t pathType;     // 6
    uint8_t rideIndex;    // 7
public:
    uint8_t GetEntranceType() const;
    uint8_t GetRideIndex() const;
    uint8_t GetStationIndex() const;
    uint8_t GetSequenceIndex() const;
    uint8_t GetPathType() const;

    void SetEntranceType(uint8_t newType);
    void SetRideIndex(uint8_t newRideIndex);
    void SetStationIndex(uint8_t stationIndex);
    void SetSequenceIndex(uint8_t newSequenceIndex);
    void SetPathType(uint8_t newPathType);
};
assert_struct_size(RCT12EntranceElement, 8);
struct RCT12BannerElement : RCT12TileElementBase
{
private:
    uint8_t index;    // 4
    uint8_t position; // 5
    uint8_t flags;    // 6
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t unused; // 7
#pragma clang diagnostic pop
public:
    uint8_t GetIndex() const;
    uint8_t GetPosition() const;
    uint8_t GetAllowedEdges() const;

    void SetIndex(uint8_t newIndex);
    void SetPosition(uint8_t newPosition);
    void SetAllowedEdges(uint8_t newEdges);
};
assert_struct_size(RCT12BannerElement, 8);

struct RCT12CorruptElement : RCT12TileElementBase
{
    uint8_t pad[4];
};
assert_struct_size(RCT12CorruptElement, 8);

struct RCT12EightCarsCorruptElement14 : RCT12TileElementBase
{
    uint8_t pad[4];
};
assert_struct_size(RCT12EightCarsCorruptElement14, 8);

struct RCT12EightCarsCorruptElement15 : RCT12TileElementBase
{
    uint8_t pad[4];
};
assert_struct_size(RCT12EightCarsCorruptElement15, 8);

struct RCT12SpriteBase
{
    uint8_t sprite_identifier;       // 0x00
    uint8_t type;                    // 0x01
    uint16_t next_in_quadrant;       // 0x02
    uint16_t next;                   // 0x04
    uint16_t previous;               // 0x06
    uint8_t linked_list_type_offset; // 0x08
    uint8_t sprite_height_negative;  // 0x09
    uint16_t sprite_index;           // 0x0A
    uint16_t flags;                  // 0x0C
    int16_t x;                       // 0x0E
    int16_t y;                       // 0x10
    int16_t z;                       // 0x12
    uint8_t sprite_width;            // 0x14
    uint8_t sprite_height_positive;  // 0x15
    int16_t sprite_left;             // 0x16
    int16_t sprite_top;              // 0x18
    int16_t sprite_right;            // 0x1A
    int16_t sprite_bottom;           // 0x1C
    uint8_t sprite_direction;        // 0x1E
};
assert_struct_size(RCT12SpriteBase, 0x1F);

struct RCT12SpriteBalloon : RCT12SpriteBase
{
    uint8_t pad_1F[0x24 - 0x1F];
    uint16_t popped;      // 0x24
    uint8_t time_to_move; // 0x26
    uint8_t frame;        // 0x27
    uint8_t pad_28[4];
    uint8_t colour; // 0x2C
};
assert_struct_size(RCT12SpriteBalloon, 0x2D);

struct RCT12SpriteDuck : RCT12SpriteBase
{
    uint8_t pad_1F[0x26 - 0x1F];
    uint16_t frame; // 0x26
    uint8_t pad_28[0x30 - 0x28];
    int16_t target_x; // 0x30
    int16_t target_y; // 0x32
    uint8_t pad_34[0x14];
    uint8_t state; // 0x48
};
assert_struct_size(RCT12SpriteDuck, 0x49);

struct RCT12SpriteLitter : RCT12SpriteBase
{
    uint8_t pad_1F[0x24 - 0x1F];
    uint32_t creationTick; // 0x24
};
assert_struct_size(RCT12SpriteLitter, 0x28);

struct RCT12SpriteParticle : RCT12SpriteBase
{
    uint8_t pad_1F[0x26 - 0x1F];
    uint16_t frame; // 0x26
};
assert_struct_size(RCT12SpriteParticle, 0x28);

struct RCT12SpriteJumpingFountain : RCT12SpriteBase
{
    uint8_t pad_1F[0x26 - 0x1F];
    uint8_t num_ticks_alive; // 0x26
    uint8_t frame;           // 0x27
    uint8_t pad_28[0x2F - 0x28];
    uint8_t fountain_flags; // 0x2F
    int16_t target_x;       // 0x30
    int16_t target_y;       // 0x32
    uint8_t pad_34[0x46 - 0x34];
    uint16_t iteration; // 0x46
};
assert_struct_size(RCT12SpriteJumpingFountain, 0x48);

struct RCT12SpriteMoneyEffect : RCT12SpriteBase
{
    uint8_t pad_1F[0x24 - 0x1F];
    uint16_t move_delay;   // 0x24
    uint8_t num_movements; // 0x26
    uint8_t vertical;
    money32 value; // 0x28
    uint8_t pad_2C[0x44 - 0x2C];
    int16_t offset_x; // 0x44
    uint16_t wiggle;  // 0x46
};
assert_struct_size(RCT12SpriteMoneyEffect, 0x48);

struct RCT12SpriteCrashedVehicleParticle : RCT12SpriteBase
{
    uint8_t pad_1F[0x24 - 0x1F];
    uint16_t time_to_live; // 0x24
    uint16_t frame;        // 0x26
    uint8_t pad_28[0x2C - 0x28];
    uint8_t colour[2];            // 0x2C
    uint16_t crashed_sprite_base; // 0x2E
    int16_t velocity_x;           // 0x30
    int16_t velocity_y;           // 0x32
    int16_t velocity_z;           // 0x34
    uint8_t pad_36[0x38 - 0x36];
    int32_t acceleration_x; // 0x38
    int32_t acceleration_y; // 0x3C
    int32_t acceleration_z; // 0x40
};
assert_struct_size(RCT12SpriteCrashedVehicleParticle, 0x44);

struct RCT12SpriteCrashSplash : RCT12SpriteBase
{
    uint8_t pad_1F[0x26 - 0x1F];
    uint16_t frame; // 0x26
};
assert_struct_size(RCT12SpriteCrashSplash, 0x28);

struct RCT12SpriteSteamParticle : RCT12SpriteBase
{
    uint8_t pad_1F[0x24 - 0x1F];
    uint16_t time_to_move; // 0x24
    uint16_t frame;        // 0x26
};
assert_struct_size(RCT12SpriteSteamParticle, 0x28);

struct RCT12PeepThought
{
    uint8_t type;
    uint8_t item;
    uint8_t freshness;
    uint8_t fresh_timeout;
};
assert_struct_size(RCT12PeepThought, 4);

struct RCT12RideMeasurement
{
    uint8_t ride_index;                                 // 0x0000
    uint8_t flags;                                      // 0x0001
    uint32_t last_use_tick;                             // 0x0002
    uint16_t num_items;                                 // 0x0006
    uint16_t current_item;                              // 0x0008
    uint8_t vehicle_index;                              // 0x000A
    uint8_t current_station;                            // 0x000B
    int8_t vertical[RCT12_RIDE_MEASUREMENT_MAX_ITEMS];  // 0x000C
    int8_t lateral[RCT12_RIDE_MEASUREMENT_MAX_ITEMS];   // 0x12CC
    uint8_t velocity[RCT12_RIDE_MEASUREMENT_MAX_ITEMS]; // 0x258C
    uint8_t altitude[RCT12_RIDE_MEASUREMENT_MAX_ITEMS]; // 0x384C
};
assert_struct_size(RCT12RideMeasurement, 0x4B0C);

struct RCT12Banner
{
    RCT12ObjectEntryIndex type;
    uint8_t flags;            // 0x01
    rct_string_id string_idx; // 0x02
    union
    {
        uint8_t colour;     // 0x04
        uint8_t ride_index; // 0x04
    };
    uint8_t text_colour; // 0x05
    uint8_t x;           // 0x06
    uint8_t y;           // 0x07
};
assert_struct_size(RCT12Banner, 8);

struct RCT12MapAnimation
{
    uint8_t baseZ;
    uint8_t type;
    uint16_t x;
    uint16_t y;
};
assert_struct_size(RCT12MapAnimation, 6);

struct RCT12ResearchItem
{
    // Bit 16 (0: scenery entry, 1: ride entry)
    union
    {
        uint32_t rawValue;
        struct
        {
            RCT12ObjectEntryIndex entryIndex;
            uint8_t baseRideType;
            uint8_t type; // 0: scenery entry, 1: ride entry
            uint8_t flags;
        };
    };
    uint8_t category;

    bool IsInventedEndMarker() const;
    bool IsRandomEndMarker() const;
    bool IsUninventedEndMarker() const;
};
assert_struct_size(RCT12ResearchItem, 5);

#pragma pack(pop)

ObjectEntryIndex RCTEntryIndexToOpenRCT2EntryIndex(const RCT12ObjectEntryIndex index);
RCT12ObjectEntryIndex OpenRCT2EntryIndexToRCTEntryIndex(const ObjectEntryIndex index);
