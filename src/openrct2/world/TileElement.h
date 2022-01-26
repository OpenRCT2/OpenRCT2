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
#include "../ride/RideTypes.h"
#include "../ride/Station.h"
#include "Banner.h"
#include "Footpath.h"
#include "tile_element/TileElementType.h"

struct Banner;
struct CoordsXY;
struct LargeSceneryEntry;
struct SmallSceneryEntry;
struct WallSceneryEntry;
struct PathBitEntry;
struct BannerSceneryEntry;
struct rct_footpath_entry;
class LargeSceneryObject;
class TerrainSurfaceObject;
class TerrainEdgeObject;
class FootpathObject;
class FootpathSurfaceObject;
class FootpathRailingsObject;
using track_type_t = uint16_t;

constexpr const uint8_t MAX_ELEMENT_HEIGHT = 255;
constexpr const uint8_t OWNER_MASK = 0b00001111;

#pragma pack(push, 1)

struct TileElement;
struct SurfaceElement;
struct PathElement;
struct TrackElement;
struct SmallSceneryElement;
struct LargeSceneryElement;
struct WallElement;
struct EntranceElement;
struct BannerElement;

struct TileElementBase
{
    uint8_t type;             // 0
    uint8_t Flags;            // 1. Upper nibble: flags. Lower nibble: occupied quadrants (one bit per quadrant).
    uint8_t base_height;      // 2
    uint8_t clearance_height; // 3
    uint8_t owner;            // 4

    void Remove();

    TileElementType GetType() const;
    void SetType(TileElementType newType);

    Direction GetDirection() const;
    void SetDirection(Direction direction);
    Direction GetDirectionWithOffset(uint8_t offset) const;

    bool IsLastForTile() const;
    void SetLastForTile(bool on);
    bool IsGhost() const;
    void SetGhost(bool isGhost);
    bool IsInvisible() const;
    void SetInvisible(bool on);

    uint8_t GetOccupiedQuadrants() const;
    void SetOccupiedQuadrants(uint8_t quadrants);

    int32_t GetBaseZ() const;
    void SetBaseZ(int32_t newZ);

    int32_t GetClearanceZ() const;
    void SetClearanceZ(int32_t newZ);

    uint8_t GetOwner() const;
    void SetOwner(uint8_t newOwner);

    template<typename TType> const TType* as() const
    {
        if constexpr (std::is_same_v<TType, TileElement>)
            return reinterpret_cast<const TileElement*>(this);
        else
            return GetType() == TType::ElementType ? reinterpret_cast<const TType*>(this) : nullptr;
    }

    template<typename TType> TType* as()
    {
        if constexpr (std::is_same_v<TType, TileElement>)
            return reinterpret_cast<TileElement*>(this);
        else
            return GetType() == TType::ElementType ? reinterpret_cast<TType*>(this) : nullptr;
    }

    const SurfaceElement* AsSurface() const
    {
        return as<SurfaceElement>();
    }
    SurfaceElement* AsSurface()
    {
        return as<SurfaceElement>();
    }
    const PathElement* AsPath() const
    {
        return as<PathElement>();
    }
    PathElement* AsPath()
    {
        return as<PathElement>();
    }
    const TrackElement* AsTrack() const
    {
        return as<TrackElement>();
    }
    TrackElement* AsTrack()
    {
        return as<TrackElement>();
    }
    const SmallSceneryElement* AsSmallScenery() const
    {
        return as<SmallSceneryElement>();
    }
    SmallSceneryElement* AsSmallScenery()
    {
        return as<SmallSceneryElement>();
    }
    const LargeSceneryElement* AsLargeScenery() const
    {
        return as<LargeSceneryElement>();
    }
    LargeSceneryElement* AsLargeScenery()
    {
        return as<LargeSceneryElement>();
    }
    const WallElement* AsWall() const
    {
        return as<WallElement>();
    }
    WallElement* AsWall()
    {
        return as<WallElement>();
    }
    const EntranceElement* AsEntrance() const
    {
        return as<EntranceElement>();
    }
    EntranceElement* AsEntrance()
    {
        return as<EntranceElement>();
    }
    const BannerElement* AsBanner() const
    {
        return as<BannerElement>();
    }
    BannerElement* AsBanner()
    {
        return as<BannerElement>();
    }
};

/**
 * Map element structure
 * size: 0x10
 */
struct TileElement : public TileElementBase
{
    uint8_t pad_05[3];
    uint8_t pad_08[8];

    void ClearAs(TileElementType newType);

    RideId GetRideIndex() const;

    void SetBannerIndex(BannerIndex newIndex);
    void RemoveBannerEntry();
    BannerIndex GetBannerIndex() const;
};
assert_struct_size(TileElement, 16);

struct SurfaceElement : TileElementBase
{
    static constexpr TileElementType ElementType = TileElementType::Surface;

private:
    uint8_t Slope;
    uint8_t WaterHeight;
    uint8_t GrassLength;
    uint8_t Ownership;
    uint8_t SurfaceStyle;
    uint8_t EdgeStyle;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad_0B[5];
#pragma clang diagnostic pop

public:
    uint8_t GetSlope() const;
    void SetSlope(uint8_t newSlope);

    uint32_t GetSurfaceStyle() const;
    TerrainSurfaceObject* GetSurfaceStyleObject() const;
    void SetSurfaceStyle(uint32_t newStyle);
    uint32_t GetEdgeStyle() const;
    TerrainEdgeObject* GetEdgeStyleObject() const;
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
    static constexpr TileElementType ElementType = TileElementType::Path;

private:
    ObjectEntryIndex SurfaceIndex;  // 5
    ObjectEntryIndex RailingsIndex; // 7
    uint8_t Additions;              // 9 (0 means no addition)
    uint8_t EdgesAndCorners;        // 10 (edges in lower 4 bits, corners in upper 4)
    uint8_t Flags2;                 // 11
    uint8_t SlopeDirection;         // 12
    union
    {
        uint8_t AdditionStatus; // 13, only used for litter bins
        RideId rideIndex;       // 13
    };
    ::StationIndex StationIndex; // 15

public:
    ObjectEntryIndex GetLegacyPathEntryIndex() const;
    const FootpathObject* GetLegacyPathEntry() const;
    void SetLegacyPathEntryIndex(ObjectEntryIndex newIndex);
    bool HasLegacyPathEntry() const;

    ObjectEntryIndex GetSurfaceEntryIndex() const;
    const FootpathSurfaceObject* GetSurfaceEntry() const;
    void SetSurfaceEntryIndex(ObjectEntryIndex newIndex);

    ObjectEntryIndex GetRailingsEntryIndex() const;
    const FootpathRailingsObject* GetRailingsEntry() const;
    void SetRailingsEntryIndex(ObjectEntryIndex newIndex);

    const PathSurfaceDescriptor* GetSurfaceDescriptor() const;
    const PathRailingsDescriptor* GetRailingsDescriptor() const;

    uint8_t GetQueueBannerDirection() const;
    void SetQueueBannerDirection(uint8_t direction);

    bool IsSloped() const;
    void SetSloped(bool isSloped);

    Direction GetSlopeDirection() const;
    void SetSlopeDirection(Direction newSlope);

    RideId GetRideIndex() const;
    void SetRideIndex(RideId newRideIndex);

    ::StationIndex GetStationIndex() const;
    void SetStationIndex(::StationIndex newStationIndex);

    bool IsWide() const;
    void SetWide(bool isWide);

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
    ObjectEntryIndex GetAdditionEntryIndex() const;
    PathBitEntry* GetAdditionEntry() const;
    void SetAddition(uint8_t newAddition);

    bool AdditionIsGhost() const;
    void SetAdditionIsGhost(bool isGhost);

    uint8_t GetAdditionStatus() const;
    void SetAdditionStatus(uint8_t newStatus);

    bool ShouldDrawPathOverSupports() const;
    void SetShouldDrawPathOverSupports(bool on);

    bool IsLevelCrossing(const CoordsXY& coords) const;
};
assert_struct_size(PathElement, 16);

struct TrackElement : TileElementBase
{
    static constexpr TileElementType ElementType = TileElementType::Track;

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
            StationIndex stationIndex;
        };
        struct
        {
            uint16_t MazeEntry; // 6
        };
    };
    uint8_t Flags2;
    RideId RideIndex;
    ride_type_t RideType;

public:
    track_type_t GetTrackType() const;
    void SetTrackType(track_type_t newEntryIndex);

    ride_type_t GetRideType() const;
    void SetRideType(const ride_type_t rideType);

    uint8_t GetSequenceIndex() const;
    void SetSequenceIndex(uint8_t newSequenceIndex);

    RideId GetRideIndex() const;
    void SetRideIndex(RideId newRideIndex);

    uint8_t GetColourScheme() const;
    void SetColourScheme(uint8_t newColourScheme);

    StationIndex GetStationIndex() const;
    void SetStationIndex(StationIndex newStationIndex);

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

    bool IsStation() const;
    bool IsBlockStart() const;
};
assert_struct_size(TrackElement, 16);

struct SmallSceneryElement : TileElementBase
{
    static constexpr TileElementType ElementType = TileElementType::SmallScenery;

private:
    ObjectEntryIndex entryIndex; // 5
    uint8_t age;                 // 7
    uint8_t colour_1;            // 8
    uint8_t colour_2;            // 9
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad_0A[6];
#pragma clang diagnostic pop

public:
    ObjectEntryIndex GetEntryIndex() const;
    void SetEntryIndex(ObjectEntryIndex newIndex);
    SmallSceneryEntry* GetEntry() const;
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
    static constexpr TileElementType ElementType = TileElementType::LargeScenery;

private:
    ObjectEntryIndex EntryIndex;
    ::BannerIndex BannerIndex;
    uint8_t SequenceIndex;
    uint8_t Colour[3];
    uint8_t Flags2;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad[2];
#pragma clang diagnostic pop

public:
    ObjectEntryIndex GetEntryIndex() const;
    void SetEntryIndex(ObjectEntryIndex newIndex);
    LargeSceneryEntry* GetEntry() const;
    const LargeSceneryObject* GetObject() const;

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
    static constexpr TileElementType ElementType = TileElementType::Wall;

private:
    ObjectEntryIndex entryIndex; // 05
    colour_t colour_1;           // 07
    colour_t colour_2;           // 08
    colour_t colour_3;           // 09
    BannerIndex banner_index;    // 0A
    uint8_t animation;           // 0C 0b_dfff_ft00 d = direction, f = frame num, t = across track flag (not used)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad_0D[3];
#pragma clang diagnostic pop

public:
    uint16_t GetEntryIndex() const;
    void SetEntryIndex(uint16_t newIndex);
    WallSceneryEntry* GetEntry() const;

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
};
assert_struct_size(WallElement, 16);

struct EntranceElement : TileElementBase
{
    static constexpr TileElementType ElementType = TileElementType::Entrance;

private:
    uint8_t entranceType;      // 5
    uint8_t SequenceIndex;     // 6. Only uses the lower nibble.
    StationIndex stationIndex; // 7
    ObjectEntryIndex PathType; // 8
    RideId rideIndex;          // A
    uint8_t flags2;            // C
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad_0D[3];
#pragma clang diagnostic pop

public:
    uint8_t GetEntranceType() const;
    void SetEntranceType(uint8_t newType);

    RideId GetRideIndex() const;
    void SetRideIndex(RideId newRideIndex);

    StationIndex GetStationIndex() const;
    void SetStationIndex(StationIndex newStationIndex);

    uint8_t GetSequenceIndex() const;
    void SetSequenceIndex(uint8_t newSequenceIndex);

    bool HasLegacyPathEntry() const;

    ObjectEntryIndex GetLegacyPathEntryIndex() const;
    const FootpathObject* GetLegacyPathEntry() const;
    void SetLegacyPathEntryIndex(ObjectEntryIndex newPathType);

    ObjectEntryIndex GetSurfaceEntryIndex() const;
    const FootpathSurfaceObject* GetSurfaceEntry() const;
    void SetSurfaceEntryIndex(ObjectEntryIndex newIndex);

    const PathSurfaceDescriptor* GetPathSurfaceDescriptor() const;

    int32_t GetDirections() const;
};
assert_struct_size(EntranceElement, 16);

struct BannerElement : TileElementBase
{
    static constexpr TileElementType ElementType = TileElementType::Banner;

private:
    BannerIndex index;    // 5
    uint8_t position;     // 7
    uint8_t AllowedEdges; // 8
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    uint8_t pad_09[7];
#pragma clang diagnostic pop
public:
    Banner* GetBanner() const;
    BannerSceneryEntry* GetEntry() const;

    BannerIndex GetIndex() const;
    void SetIndex(BannerIndex newIndex);

    uint8_t GetPosition() const;
    void SetPosition(uint8_t newPosition);

    uint8_t GetAllowedEdges() const;
    void SetAllowedEdges(uint8_t newEdges);
    void ResetAllowedEdges();
};
assert_struct_size(BannerElement, 16);

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
    TILE_ELEMENT_FLAG_INVISIBLE = (1 << 5),
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

enum
{
    LANDSCAPE_DOOR_CLOSED = 0,
    LANDSCAPE_DOOR_HALF_OPEN = 2,
    LANDSCAPE_DOOR_OPEN = 3,
};

bool tile_element_is_underground(TileElement* tileElement);
