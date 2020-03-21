/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../ride/RideTypes.h"
#include "../ride/Station.h"
#include "Banner.h"
#include "Footpath.h"
#include "Location.hpp"

struct Banner;
struct rct_scenery_entry;
struct rct_footpath_entry;
using track_type_t = uint16_t;

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
    uint8_t Flags;            // 1. Upper nibble: flags. Lower nibble: occupied quadrants (one bit per quadrant).
    uint8_t base_height;      // 2
    uint8_t clearance_height; // 3

    void Remove();

    uint8_t GetType() const;
    void SetType(uint8_t newType);

    Direction GetDirection() const;
    void SetDirection(Direction direction);
    Direction GetDirectionWithOffset(uint8_t offset) const;

    bool IsLastForTile() const;
    void SetLastForTile(bool on);
    bool IsGhost() const;
    void SetGhost(bool isGhost);

    uint8_t GetOccupiedQuadrants() const;
    void SetOccupiedQuadrants(uint8_t quadrants);

    int32_t GetBaseZ() const;
    void SetBaseZ(int32_t newZ);

    int32_t GetClearanceZ() const;
    void SetClearanceZ(int32_t newZ);
};

/**
 * Map element structure
 * size: 0x10
 */
struct TileElement : public TileElementBase
{
    uint8_t pad_04[4];
    uint8_t pad_08[8];

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

    void ClearAs(uint8_t newType);
};
assert_struct_size(TileElement, 16);

struct SurfaceElement : TileElementBase
{
private:
    uint8_t Slope;
    uint8_t WaterHeight;
    uint8_t GrassLength;
    uint8_t Ownership;
    uint8_t SurfaceStyle;
    uint8_t EdgeStyle;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad_0A[6];
#pragma clang diagnostic pop

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
    void SetGrassLengthAndInvalidate(uint8_t newLength, const CoordsXY& coords);
    void UpdateGrassLength(const CoordsXY& coords);

    uint8_t GetOwnership() const;
    void SetOwnership(uint8_t newOwnership);

    int32_t GetWaterHeight() const;
    void SetWaterHeight(int32_t newWaterHeight);

    uint8_t GetParkFences() const;
    void SetParkFences(uint8_t newParkFences);

    bool HasTrackThatNeedsWater() const;
    void SetHasTrackThatNeedsWater(bool on);
};
assert_struct_size(SurfaceElement, 16);

struct PathElement : TileElementBase
{
private:
    PathSurfaceIndex SurfaceIndex; // 4
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    PathRailingsIndex RailingsIndex; // 6
#pragma clang diagnostic pop
    uint8_t Additions;      // 7 (0 means no addition)
    uint8_t Edges;          // 8
    uint8_t Flags2;         // 9
    uint8_t SlopeDirection; // 10

    union
    {
        uint8_t AdditionStatus; // 11, only used for litter bins
        ride_id_t rideIndex;    // 11
    };
    ::StationIndex StationIndex; // 13
    uint8_t isWideFlags;         // 14
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad_0E[1];
#pragma clang diagnostic pop

public:
    PathSurfaceIndex GetSurfaceEntryIndex() const;
    PathSurfaceEntry* GetSurfaceEntry() const;
    void SetSurfaceEntryIndex(PathSurfaceIndex newIndex);

    PathRailingsIndex GetRailingEntryIndex() const;
    PathRailingsEntry* GetRailingEntry() const;
    void SetRailingEntryIndex(PathRailingsIndex newIndex);

    uint8_t GetQueueBannerDirection() const;
    void SetQueueBannerDirection(uint8_t direction);

    bool IsSloped() const;
    void SetSloped(bool isSloped);

    Direction GetSlopeDirection() const;
    void SetSlopeDirection(Direction newSlope);

    ride_id_t GetRideIndex() const;
    void SetRideIndex(ride_id_t newRideIndex);

    ::StationIndex GetStationIndex() const;
    void SetStationIndex(::StationIndex newStationIndex);

    bool IsWide() const;
    bool IsWideForGroup(uint8_t wideGroup) const;
    uint8_t GetWideFlags() const;
    void SetWide(bool isWide);
    void SetWideForGroup(uint8_t wideGroup, bool isWide);

    bool IsQueue() const;
    void SetIsQueue(bool isQueue);
    bool HasQueueBanner() const;
    void SetHasQueueBanner(bool hasQueueBanner);

    bool IsBroken() const;
    void SetIsBroken(bool isBroken);

    bool IsBlockedByVehicle() const;
    void SetIsBlockedByVehicle(bool isBlocked);

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

    bool ShouldDrawPathOverSupports();
    void SetShouldDrawPathOverSupports(bool on);

    bool IsLevelCrossing(const CoordsXY& coords) const;
};
assert_struct_size(PathElement, 16);

struct TrackElement : TileElementBase
{
private:
    track_type_t TrackType;
    union
    {
        struct
        {
            uint8_t Sequence;
            uint8_t ColourScheme;
            union
            {
                // - Bits 3 and 4 are never set
                // - Bits 1 and 2 are set when a vehicle triggers the on-ride photo and act like a countdown from 3.
                // - If any of the bits 1-4 are set, the game counts it as a photo being taken.
                uint8_t OnridePhotoBits;
                // Contains the brake/booster speed, divided by 2.
                uint8_t BrakeBoosterSpeed;
            };
            uint8_t StationIndex;
        };
        struct
        {
            uint16_t MazeEntry; // 5
        };
    };
    uint8_t Flags2;
    ride_id_t RideIndex;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad[3];
#pragma clang diagnostic pop

public:
    track_type_t GetTrackType() const;
    void SetTrackType(track_type_t newEntryIndex);

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

    bool BlockBrakeClosed() const;
    void SetBlockBrakeClosed(bool isClosed);

    bool IsIndestructible() const;
    void SetIsIndestructible(bool isIndestructible);

    uint8_t GetBrakeBoosterSpeed() const;
    void SetBrakeBoosterSpeed(uint8_t speed);

    bool HasGreenLight() const;
    void SetHasGreenLight(bool on);

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
    uint8_t GetPhotoTimeout() const;

    bool IsHighlighted() const;
    void SetHighlight(bool on);

    // Used by ghost train, RCT1 feature, will be reintroduced at some point.
    // (See https://github.com/OpenRCT2/OpenRCT2/issues/7059)
    uint8_t GetDoorAState() const;
    uint8_t GetDoorBState() const;
    void SetDoorAState(uint8_t newState);
    void SetDoorBState(uint8_t newState);
};
assert_struct_size(TrackElement, 16);

struct SmallSceneryElement : TileElementBase
{
private:
    ObjectEntryIndex entryIndex; // 4
    uint8_t age;                 // 5
    uint8_t colour_1;            // 6
    uint8_t colour_2;            // 7
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad_09[7];
#pragma clang diagnostic pop

public:
    ObjectEntryIndex GetEntryIndex() const;
    void SetEntryIndex(ObjectEntryIndex newIndex);
    rct_scenery_entry* GetEntry() const;
    uint8_t GetAge() const;
    void SetAge(uint8_t newAge);
    void IncreaseAge(const CoordsXY& sceneryPos);
    uint8_t GetSceneryQuadrant() const;
    void SetSceneryQuadrant(uint8_t newQuadrant);
    colour_t GetPrimaryColour() const;
    void SetPrimaryColour(colour_t colour);
    colour_t GetSecondaryColour() const;
    void SetSecondaryColour(colour_t colour);
    bool NeedsSupports() const;
    void SetNeedsSupports();
    void UpdateAge(const CoordsXY& sceneryPos);
};
assert_struct_size(SmallSceneryElement, 16);

struct LargeSceneryElement : TileElementBase
{
private:
    uint16_t EntryIndex;
    ::BannerIndex BannerIndex;
    uint8_t SequenceIndex;
    uint8_t Colour[3];
    uint8_t Flags2;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad[3];
#pragma clang diagnostic pop

public:
    uint16_t GetEntryIndex() const;
    void SetEntryIndex(uint16_t newIndex);
    rct_scenery_entry* GetEntry() const;

    uint8_t GetSequenceIndex() const;
    void SetSequenceIndex(uint8_t newIndex);

    colour_t GetPrimaryColour() const;
    void SetPrimaryColour(colour_t colour);
    colour_t GetSecondaryColour() const;
    void SetSecondaryColour(colour_t colour);

    Banner* GetBanner() const;
    ::BannerIndex GetBannerIndex() const;
    void SetBannerIndex(::BannerIndex newIndex);

    bool IsAccounted() const;
    void SetIsAccounted(bool isAccounted);
};
assert_struct_size(LargeSceneryElement, 16);

struct WallElement : TileElementBase
{
private:
    ObjectEntryIndex entryIndex; // 04
    colour_t colour_1;           // 06
    colour_t colour_2;           // 07
    colour_t colour_3;           // 08
    BannerIndex banner_index;    // 09
    uint8_t animation;           // 0A 0b_dfff_ft00 d = direction, f = frame num, t = across track flag (not used)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad_0C[4];
#pragma clang diagnostic pop

public:
    uint16_t GetEntryIndex() const;
    void SetEntryIndex(uint16_t newIndex);
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

    Banner* GetBanner() const;
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
assert_struct_size(WallElement, 16);

struct EntranceElement : TileElementBase
{
private:
    uint8_t entranceType;      // 4
    uint8_t SequenceIndex;     // 5. Only uses the lower nibble.
    uint8_t StationIndex;      // 6
    PathSurfaceIndex PathType; // 7
    ride_id_t rideIndex;       // 8
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad_0B[5];
#pragma clang diagnostic pop

public:
    uint8_t GetEntranceType() const;
    void SetEntranceType(uint8_t newType);

    ride_id_t GetRideIndex() const;
    void SetRideIndex(ride_id_t newRideIndex);

    uint8_t GetStationIndex() const;
    void SetStationIndex(uint8_t newStationIndex);

    uint8_t GetSequenceIndex() const;
    void SetSequenceIndex(uint8_t newSequenceIndex);

    PathSurfaceIndex GetPathType() const;
    void SetPathType(PathSurfaceIndex newPathType);
};
assert_struct_size(EntranceElement, 16);

struct BannerElement : TileElementBase
{
private:
    BannerIndex index; // 4
    uint8_t position;  // 5
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t flags;  // 6
    uint8_t unused; // 7
    uint8_t pad_09[7];
#pragma clang diagnostic pop
public:
    Banner* GetBanner() const;
    rct_scenery_entry* GetEntry() const;

    BannerIndex GetIndex() const;
    void SetIndex(BannerIndex newIndex);

    uint8_t GetPosition() const;
    void SetPosition(uint8_t newPosition);

    uint8_t GetAllowedEdges() const;
    void SetAllowedEdges(uint8_t newEdges);
    void ResetAllowedEdges();
};
assert_struct_size(BannerElement, 16);

struct CorruptElement : TileElementBase
{
    uint8_t pad[4];
    uint8_t pad_08[8];
};
assert_struct_size(CorruptElement, 16);
#pragma pack(pop)

class QuarterTile
{
private:
    uint8_t _val{ 0 };

public:
    constexpr QuarterTile(uint8_t tileQuarter, uint8_t zQuarter)
        : _val(tileQuarter | (zQuarter << 4))
    {
    }

    QuarterTile(uint8_t tileAndZQuarter)
        : _val(tileAndZQuarter)
    {
    }

    // Rotate both of the values amount. Returns new RValue QuarterTile
    const QuarterTile Rotate(uint8_t amount) const;

    uint8_t GetBaseQuarterOccupied() const
    {
        return _val & 0xF;
    }

    uint8_t GetZQuarterOccupied() const
    {
        return (_val >> 4) & 0xF;
    }
};

enum
{
    TILE_ELEMENT_QUADRANT_SW,
    TILE_ELEMENT_QUADRANT_NW,
    TILE_ELEMENT_QUADRANT_NE,
    TILE_ELEMENT_QUADRANT_SE
};

enum
{
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
#define TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK 0b00001111

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
