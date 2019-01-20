/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../ride/RideTypes.h"
#include "Footpath.h"
#include "Location.hpp"

struct rct_scenery_entry;
struct rct_footpath_entry;

#pragma pack(push, 1)

enum
{
    TILE_ELEMENT_TYPE_SURFACE = (0 << 2),
    TILE_ELEMENT_TYPE_PATH = (1 << 2),
    TILE_ELEMENT_TYPE_TRACK = (2 << 2),
    TILE_ELEMENT_TYPE_SMALL_SCENERY = (3 << 2),
    TILE_ELEMENT_TYPE_ENTRANCE = (4 << 2),
    TILE_ELEMENT_TYPE_WALL = (5 << 2),
    TILE_ELEMENT_TYPE_LARGE_SCENERY = (6 << 2),
    TILE_ELEMENT_TYPE_BANNER = (7 << 2),
    // The corrupt element type is used for skipping drawing other following
    // elements on a given tile.
    TILE_ELEMENT_TYPE_CORRUPT = (8 << 2),
};

enum class TileElementType : uint8_t
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
};

struct SurfaceElement;
struct PathElement;
struct TrackElement;
struct SmallSceneryElement;
struct LargeSceneryElement;
struct WallElement;
struct EntranceElement;
struct BannerElement;
struct CorruptElement;

struct TileElementBase
{
    uint8_t type;             // 0
    uint8_t flags;            // 1
    uint8_t base_height;      // 2
    uint8_t clearance_height; // 3

    uint8_t GetType() const;
    void SetType(uint8_t newType);
    uint8_t GetDirection() const;
    void SetDirection(uint8_t direction);
    uint8_t GetDirectionWithOffset(uint8_t offset) const;
    bool IsLastForTile() const;
    bool IsGhost() const;
    void Remove();
};

/**
 * Map element structure
 * size: 0x08
 */
struct TileElement : public TileElementBase
{
    uint8_t pad_04[4];

    template<typename TType, TileElementType TClass> TType* as() const
    {
        return (TileElementType)GetType() == TClass ? (TType*)this : nullptr;
    }

public:
    SurfaceElement* AsSurface() const
    {
        return as<SurfaceElement, TileElementType::Surface>();
    }
    PathElement* AsPath() const
    {
        return as<PathElement, TileElementType::Path>();
    }
    TrackElement* AsTrack() const
    {
        return as<TrackElement, TileElementType::Track>();
    }
    SmallSceneryElement* AsSmallScenery() const
    {
        return as<SmallSceneryElement, TileElementType::SmallScenery>();
    }
    LargeSceneryElement* AsLargeScenery() const
    {
        return as<LargeSceneryElement, TileElementType::LargeScenery>();
    }
    WallElement* AsWall() const
    {
        return as<WallElement, TileElementType::Wall>();
    }
    EntranceElement* AsEntrance() const
    {
        return as<EntranceElement, TileElementType::Entrance>();
    }
    BannerElement* AsBanner() const
    {
        return as<BannerElement, TileElementType::Banner>();
    }
    CorruptElement* AsCorrupt() const
    {
        return as<CorruptElement, TileElementType::Corrupt>();
    }

    void ClearAs(uint8_t newType);
};
assert_struct_size(TileElement, 8);

struct SurfaceElement : TileElementBase
{
private:
    uint8_t slope;        // 4 0xE0 Edge Style, 0x1F Slope
    uint8_t terrain;      // 5 0xE0 Terrain Style, 0x1F Water height
    uint8_t grass_length; // 6
    uint8_t ownership;    // 7
public:
    uint8_t GetSlope() const;
    void SetSlope(uint8_t newSlope);

    uint32_t GetSurfaceStyle() const;
    void SetSurfaceStyle(uint32_t newStyle);
    uint32_t GetEdgeStyle() const;
    void SetEdgeStyle(uint32_t newStyle);

    bool CanGrassGrow() const;
    uint8_t GetGrassLength() const;
    void SetGrassLength(uint8_t newLength);
    void SetGrassLengthAndInvalidate(uint8_t newLength, CoordsXY coords);
    void UpdateGrassLength(CoordsXY coords);

    uint8_t GetOwnership() const;
    void SetOwnership(uint8_t newOwnership);

    uint32_t GetWaterHeight() const;
    void SetWaterHeight(uint32_t newWaterHeight);

    uint8_t GetParkFences() const;
    void SetParkFences(uint8_t newParkFences);

    bool HasTrackThatNeedsWater() const;
    void SetHasTrackThatNeedsWater(bool on);
};
assert_struct_size(SurfaceElement, 8);

struct PathElement : TileElementBase
{
private:
    uint8_t entryIndex; // 4, 0xF0 Path type, 0x08 Ride sign, 0x04 Set when path is sloped, 0x03 Rotation
    uint8_t additions;  // 5, 0bGSSSAAAA: G = Ghost, S = station index, A = addition (0 means no addition)
    uint8_t edges;      // 6
    union
    {
        uint8_t additionStatus; // 7
        ride_id_t rideIndex;
    };

public:
    uint8_t GetPathEntryIndex() const;
    PathSurfaceEntry* GetPathEntry() const;
    void SetPathEntryIndex(uint8_t newIndex);

    uint8_t GetRailingEntryIndex() const;
    PathRailingsEntry* GetRailingEntry() const;
    void SetRailingEntryIndex(uint8_t newIndex);

    uint8_t GetQueueBannerDirection() const;
    void SetQueueBannerDirection(uint8_t direction);

    bool IsSloped() const;
    void SetSloped(bool isSloped);

    uint8_t GetSlopeDirection() const;
    void SetSlopeDirection(uint8_t newSlope);

    ride_id_t GetRideIndex() const;
    void SetRideIndex(ride_id_t newRideIndex);

    uint8_t GetStationIndex() const;
    void SetStationIndex(uint8_t newStationIndex);

    bool IsWide() const;
    void SetWide(bool isWide);

    bool IsQueue() const;
    void SetIsQueue(bool isQueue);
    bool HasQueueBanner() const;
    void SetHasQueueBanner(bool hasQueueBanner);

    uint8_t GetEdges() const;
    void SetEdges(uint8_t newEdges);
    uint8_t GetCorners() const;
    void SetCorners(uint8_t newCorners);
    uint8_t GetEdgesAndCorners() const;
    void SetEdgesAndCorners(uint8_t newEdgesAndCorners);

    bool HasAddition() const;
    uint8_t GetAddition() const;
    uint8_t GetAdditionEntryIndex() const;
    rct_scenery_entry* GetAdditionEntry() const;
    void SetAddition(uint8_t newAddition);

    bool AdditionIsGhost() const;
    void SetAdditionIsGhost(bool isGhost);

    uint8_t GetAdditionStatus() const;
    void SetAdditionStatus(uint8_t newStatus);

    uint8_t GetRCT1PathType() const;

    bool ShouldDrawPathOverSupports();
    void SetShouldDrawPathOverSupports(bool on);
};
assert_struct_size(PathElement, 8);

struct TrackElement : TileElementBase
{
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
    ride_id_t rideIndex; // 7

public:
    uint8_t GetTrackType() const;
    void SetTrackType(uint8_t newEntryIndex);

    uint8_t GetSequenceIndex() const;
    void SetSequenceIndex(uint8_t newSequenceIndex);

    ride_id_t GetRideIndex() const;
    void SetRideIndex(ride_id_t newRideIndex);

    uint8_t GetColourScheme() const;
    void SetColourScheme(uint8_t newColourScheme);

    uint8_t GetStationIndex() const;
    void SetStationIndex(uint8_t newStationIndex);

    bool HasChain() const;
    void SetHasChain(bool on);

    bool HasCableLift() const;
    void SetHasCableLift(bool on);

    bool IsInverted() const;
    void SetInverted(bool inverted);

    uint8_t GetBrakeBoosterSpeed() const;
    void SetBrakeBoosterSpeed(uint8_t speed);

    uint8_t HasGreenLight() const;
    void SetHasGreenLight(uint8_t greenLight);

    uint8_t GetSeatRotation() const;
    void SetSeatRotation(uint8_t newSeatRotation);

    uint16_t GetMazeEntry() const;
    void SetMazeEntry(uint16_t newMazeEntry);
    void MazeEntryAdd(uint16_t addVal);
    void MazeEntrySubtract(uint16_t subVal);

    bool IsTakingPhoto() const;
    void SetPhotoTimeout();
    void SetPhotoTimeout(uint8_t newValue);
    void DecrementPhotoTimeout();

    bool IsHighlighted() const;
    void SetHighlight(bool on);

    // Used in RCT1, will be reintroduced at some point.
    // (See https://github.com/OpenRCT2/OpenRCT2/issues/7059)
    uint8_t GetDoorAState() const;
    uint8_t GetDoorBState() const;
};
assert_struct_size(TrackElement, 8);

struct SmallSceneryElement : TileElementBase
{
private:
    uint8_t entryIndex; // 4
    uint8_t age;        // 5
    uint8_t colour_1;   // 6
    uint8_t colour_2;   // 7
public:
    uint8_t GetEntryIndex() const;
    void SetEntryIndex(uint8_t newIndex);
    rct_scenery_entry* GetEntry() const;
    uint8_t GetAge() const;
    void SetAge(uint8_t newAge);
    void IncreaseAge(int32_t x, int32_t y);
    uint8_t GetSceneryQuadrant() const;
    void SetSceneryQuadrant(uint8_t newQuadrant);
    colour_t GetPrimaryColour() const;
    void SetPrimaryColour(colour_t colour);
    colour_t GetSecondaryColour() const;
    void SetSecondaryColour(colour_t colour);
    bool NeedsSupports() const;
    void SetNeedsSupports();
};
assert_struct_size(SmallSceneryElement, 8);

struct LargeSceneryElement : TileElementBase
{
private:
    uint16_t entryIndex; // 4
    uint8_t colour[2];   // 6
public:
    uint32_t GetEntryIndex() const;
    void SetEntryIndex(uint32_t newIndex);
    rct_scenery_entry* GetEntry() const;

    uint16_t GetSequenceIndex() const;
    void SetSequenceIndex(uint16_t newIndex);

    colour_t GetPrimaryColour() const;
    void SetPrimaryColour(colour_t colour);
    colour_t GetSecondaryColour() const;
    void SetSecondaryColour(colour_t colour);

    BannerIndex GetBannerIndex() const;
    void SetBannerIndex(BannerIndex newIndex);
};
assert_struct_size(LargeSceneryElement, 8);

struct WallElement : TileElementBase
{
private:
    uint8_t entryIndex; // 4
    union
    {
        uint8_t colour_3;         // 5
        BannerIndex banner_index; // 5
    };
    uint8_t colour_1;  // 6 0b_2221_1111 2 = colour_2 (uses flags for rest of colour2), 1 = colour_1
    uint8_t animation; // 7 0b_dfff_ft00 d = direction, f = frame num, t = across track flag (not used)

public:
    uint8_t GetEntryIndex() const;
    void SetEntryIndex(uint8_t newIndex);
    rct_scenery_entry* GetEntry() const;

    uint8_t GetSlope() const;
    void SetSlope(uint8_t newslope);

    colour_t GetPrimaryColour() const;
    void SetPrimaryColour(colour_t newColour);
    colour_t GetSecondaryColour() const;
    void SetSecondaryColour(colour_t newColour);
    colour_t GetTertiaryColour() const;
    void SetTertiaryColour(colour_t newColour);

    uint8_t GetAnimationFrame() const;
    void SetAnimationFrame(uint8_t frameNum);

    BannerIndex GetBannerIndex() const;
    void SetBannerIndex(BannerIndex newIndex);

    bool IsAcrossTrack() const;
    void SetAcrossTrack(bool acrossTrack);
    bool AnimationIsBackwards() const;
    void SetAnimationIsBackwards(bool isBackwards);

    void SetRawRCT1Data(uint32_t rawData);
    int32_t GetRCT1WallType(int32_t edge) const;
    colour_t GetRCT1WallColour() const;
};
assert_struct_size(WallElement, 8);

struct EntranceElement : TileElementBase
{
private:
    uint8_t entranceType; // 4
    uint8_t index;        // 5. 0bUSSS????, S = station index.
    uint8_t pathType;     // 6
    ride_id_t rideIndex;  // 7

public:
    uint8_t GetEntranceType() const;
    void SetEntranceType(uint8_t newType);

    ride_id_t GetRideIndex() const;
    void SetRideIndex(ride_id_t newRideIndex);

    uint8_t GetStationIndex() const;
    void SetStationIndex(uint8_t stationIndex);

    uint8_t GetSequenceIndex() const;
    void SetSequenceIndex(uint8_t newSequenceIndex);

    uint8_t GetPathType() const;
    void SetPathType(uint8_t newPathType);
};
assert_struct_size(EntranceElement, 8);

struct BannerElement : TileElementBase
{
private:
    BannerIndex index; // 4
    uint8_t position;  // 5
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t flags;  // 6
    uint8_t unused; // 7
#pragma clang diagnostic pop
public:
    BannerIndex GetIndex() const;
    void SetIndex(BannerIndex newIndex);

    uint8_t GetPosition() const;
    void SetPosition(uint8_t newPosition);

    uint8_t GetAllowedEdges() const;
    void SetAllowedEdges(uint8_t newEdges);
    void ResetAllowedEdges();
};
assert_struct_size(BannerElement, 8);

struct CorruptElement : TileElementBase
{
    uint8_t pad[4];
};
assert_struct_size(CorruptElement, 8);
#pragma pack(pop)

enum
{
    TILE_ELEMENT_QUADRANT_SW,
    TILE_ELEMENT_QUADRANT_NW,
    TILE_ELEMENT_QUADRANT_NE,
    TILE_ELEMENT_QUADRANT_SE
};

enum
{
    TILE_ELEMENT_TYPE_FLAG_HIGHLIGHT = (1 << 6),
    SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER = (1 << 6),
};

enum
{
    TILE_ELEMENT_DIRECTION_WEST,
    TILE_ELEMENT_DIRECTION_NORTH,
    TILE_ELEMENT_DIRECTION_EAST,
    TILE_ELEMENT_DIRECTION_SOUTH
};

enum
{
    TILE_ELEMENT_FLAG_GHOST = (1 << 4),
    TILE_ELEMENT_FLAG_BROKEN = (1 << 5),
    TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED = (1 << 5),
    TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE = (1 << 6),
    TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE = (1 << 6),
    TILE_ELEMENT_FLAG_LAST_TILE = (1 << 7)
};

enum
{
    ENTRANCE_TYPE_RIDE_ENTRANCE,
    ENTRANCE_TYPE_RIDE_EXIT,
    ENTRANCE_TYPE_PARK_ENTRANCE
};

enum
{
    ELEMENT_IS_ABOVE_GROUND = 1 << 0,
    ELEMENT_IS_UNDERGROUND = 1 << 1,
    ELEMENT_IS_UNDERWATER = 1 << 2,
};

enum
{
    MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT = (1 << 7),
};

#define TILE_ELEMENT_QUADRANT_MASK 0b11000000
#define TILE_ELEMENT_TYPE_MASK 0b00111100
#define TILE_ELEMENT_DIRECTION_MASK 0b00000011

#define TILE_ELEMENT_COLOUR_MASK 0b00011111

#define MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK 0b01110000
#define MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK 0b00001111
#define MAP_ELEM_TRACK_SEQUENCE_TAKING_PHOTO_MASK 0b11110000

BannerIndex tile_element_get_banner_index(TileElement* tileElement);
bool tile_element_is_underground(TileElement* tileElement);

// ~Oli414: The banner functions should probably be part of banner.
void tile_element_set_banner_index(TileElement* tileElement, BannerIndex bannerIndex);
void tile_element_remove_banner_entry(TileElement* tileElement);

uint8_t tile_element_get_ride_index(const TileElement* tileElement);
