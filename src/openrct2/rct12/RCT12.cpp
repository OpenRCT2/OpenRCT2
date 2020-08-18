/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RCT12.h"

#include "../localisation/Localisation.h"
#include "../ride/Track.h"
#include "../world/Banner.h"
#include "../world/Footpath.h"
#include "../world/LargeScenery.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"
#include "../world/TileElement.h"
#include "../world/Wall.h"

uint8_t RCT12TileElementBase::GetType() const
{
    return this->type & TILE_ELEMENT_TYPE_MASK;
}

uint8_t RCT12TileElementBase::GetDirection() const
{
    return this->type & TILE_ELEMENT_DIRECTION_MASK;
}

uint8_t RCT12TileElementBase::GetOccupiedQuadrants() const
{
    return flags & TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK;
}

void RCT12TileElementBase::SetOccupiedQuadrants(uint8_t quadrants)
{
    flags &= ~TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK;
    flags |= (quadrants & TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK);
}

bool RCT12TileElementBase::IsLastForTile() const
{
    return (this->flags & RCT12_TILE_ELEMENT_FLAG_LAST_TILE) != 0;
}

bool RCT12TileElementBase::IsGhost() const
{
    return (this->flags & RCT12_TILE_ELEMENT_FLAG_GHOST) != 0;
}

void RCT12TileElementBase::SetLastForTile(bool on)
{
    if (on)
        flags |= RCT12_TILE_ELEMENT_FLAG_LAST_TILE;
    else
        flags &= ~RCT12_TILE_ELEMENT_FLAG_LAST_TILE;
}

void RCT12TileElementBase::SetGhost(bool isGhost)
{
    if (isGhost)
    {
        this->flags |= RCT12_TILE_ELEMENT_FLAG_GHOST;
    }
    else
    {
        this->flags &= ~RCT12_TILE_ELEMENT_FLAG_GHOST;
    }
}

uint8_t RCT12SurfaceElement::GetSlope() const
{
    return (slope & TILE_ELEMENT_SURFACE_SLOPE_MASK);
}

uint32_t RCT12SurfaceElement::GetSurfaceStyle() const
{
    uint32_t retVal = (terrain >> 5) & 7;
    if (type & 1)
        retVal |= (1 << 3);
    return retVal;
}

uint32_t RCT12SurfaceElement::GetEdgeStyle() const
{
    uint32_t terrain_edge = (slope >> 5) & 7;
    if (type & 128)
        terrain_edge |= (1 << 3);
    return terrain_edge;
}

uint8_t RCT12SurfaceElement::GetGrassLength() const
{
    return grass_length;
}

uint8_t RCT12SurfaceElement::GetOwnership() const
{
    return (ownership & TILE_ELEMENT_SURFACE_OWNERSHIP_MASK);
}

uint32_t RCT12SurfaceElement::GetWaterHeight() const
{
    return (terrain & RCT12_TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK) * 16;
}

uint8_t RCT12SurfaceElement::GetParkFences() const
{
    return (ownership & TILE_ELEMENT_SURFACE_PARK_FENCE_MASK);
}

bool RCT12SurfaceElement::HasTrackThatNeedsWater() const
{
    return (type & SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER) != 0;
}

uint8_t RCT12PathElement::GetEntryIndex() const
{
    return (entryIndex & RCT12_FOOTPATH_PROPERTIES_TYPE_MASK) >> 4;
}

uint8_t RCT12PathElement::GetQueueBannerDirection() const
{
    return ((type & FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK) >> 6);
}

bool RCT12PathElement::IsSloped() const
{
    return (entryIndex & RCT12_FOOTPATH_PROPERTIES_FLAG_IS_SLOPED) != 0;
}

uint8_t RCT12PathElement::GetSlopeDirection() const
{
    return entryIndex & RCT12_FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
}

uint8_t RCT12PathElement::GetRideIndex() const
{
    return rideIndex;
}

uint8_t RCT12PathElement::GetStationIndex() const
{
    return (additions & RCT12_FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK) >> 4;
}

bool RCT12PathElement::IsWide() const
{
    return (type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE) != 0;
}

bool RCT12PathElement::IsQueue() const
{
    return (type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE) != 0;
}

bool RCT12PathElement::HasQueueBanner() const
{
    return (entryIndex & RCT12_FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER) != 0;
}
uint8_t RCT12PathElement::GetEdges() const
{
    return edges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK;
}

uint8_t RCT12PathElement::GetCorners() const
{
    return edges >> 4;
}

uint8_t RCT12PathElement::GetAddition() const
{
    return additions & RCT12_FOOTPATH_PROPERTIES_ADDITIONS_TYPE_MASK;
}

bool RCT12PathElement::AdditionIsGhost() const
{
    return (additions & RCT12_FOOTPATH_PROPERTIES_ADDITIONS_FLAG_GHOST) != 0;
}

uint8_t RCT12PathElement::GetAdditionStatus() const
{
    return additionStatus;
}

uint8_t RCT12PathElement::GetRCT1PathType() const
{
    uint8_t pathColour = type & 3;
    uint8_t pathType2 = (entryIndex & RCT12_FOOTPATH_PROPERTIES_TYPE_MASK) >> 2;

    pathType2 = pathType2 | pathColour;
    return pathType2;
}

uint8_t RCT12PathElement::GetRCT1SupportType() const
{
    return (flags & 0b01100000) >> 5;
}

uint8_t RCT12TrackElement::GetTrackType() const
{
    return trackType;
}

uint8_t RCT12TrackElement::GetSequenceIndex() const
{
    return sequence & RCT12_TRACK_ELEMENT_SEQUENCE_SEQUENCE_MASK;
}

uint8_t RCT12TrackElement::GetRideIndex() const
{
    return rideIndex;
}

uint8_t RCT12TrackElement::GetColourScheme() const
{
    return colour & 0x3;
}

uint8_t RCT12TrackElement::GetStationIndex() const
{
    if (track_type_is_station(trackType) || trackType == TrackElemType::TowerBase)
    {
        return (sequence & RCT12_TRACK_ELEMENT_SEQUENCE_STATION_INDEX_MASK) >> 4;
    }
    return 0;
}

bool RCT12TrackElement::HasChain() const
{
    return type & RCT12_TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
}

bool RCT12TrackElement::HasCableLift() const
{
    return colour & RCT12_TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

bool RCT12TrackElement::IsInverted() const
{
    return colour & RCT12_TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
}

uint8_t RCT12TrackElement::GetBrakeBoosterSpeed() const
{
    if (TrackTypeHasSpeedSetting(GetTrackType()))
    {
        return (sequence >> 4) << 1;
    }
    return 0;
}

bool RCT12TrackElement::HasGreenLight() const
{
    if (track_type_is_station(trackType))
    {
        return (sequence & MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT) != 0;
    }
    return false;
}

uint8_t RCT12TrackElement::GetSeatRotation() const
{
    return colour >> 4;
}

uint16_t RCT12TrackElement::GetMazeEntry() const
{
    return mazeEntry;
}

uint8_t RCT12TrackElement::GetPhotoTimeout() const
{
    if (GetTrackType() == TrackElemType::OnRidePhoto)
    {
        return sequence >> 4;
    }
    return 0;
}

uint8_t RCT12TrackElement::GetDoorAState() const
{
    return (colour & RCT12_TRACK_ELEMENT_DOOR_A_MASK) >> 2;
}

uint8_t RCT12TrackElement::GetDoorBState() const
{
    return (colour & RCT12_TRACK_ELEMENT_DOOR_B_MASK) >> 5;
}

bool RCT12TrackElement::IsIndestructible() const
{
    return (flags & RCT12_TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE) != 0;
}

void RCT12TrackElement::SetIsIndestructible(bool isIndestructible)
{
    if (isIndestructible)
    {
        flags |= RCT12_TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE;
    }
    else
    {
        flags &= ~RCT12_TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE;
    }
}

uint8_t RCT12SmallSceneryElement::GetEntryIndex() const
{
    return this->entryIndex;
}

uint8_t RCT12SmallSceneryElement::GetAge() const
{
    return this->age;
}

uint8_t RCT12SmallSceneryElement::GetSceneryQuadrant() const
{
    return (this->type & TILE_ELEMENT_QUADRANT_MASK) >> 6;
}

colour_t RCT12SmallSceneryElement::GetPrimaryColour() const
{
    return colour_1 & TILE_ELEMENT_COLOUR_MASK;
}

colour_t RCT12SmallSceneryElement::GetSecondaryColour() const
{
    return colour_2 & TILE_ELEMENT_COLOUR_MASK;
}

bool RCT12SmallSceneryElement::NeedsSupports() const
{
    return colour_1 & MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS;
}

uint32_t RCT12LargeSceneryElement::GetEntryIndex() const
{
    return entryIndex & RCT12_TILE_ELEMENT_LARGE_TYPE_MASK;
}

uint16_t RCT12LargeSceneryElement::GetSequenceIndex() const
{
    return (entryIndex >> 10);
}
colour_t RCT12LargeSceneryElement::GetPrimaryColour() const
{
    return colour[0] & TILE_ELEMENT_COLOUR_MASK;
}

colour_t RCT12LargeSceneryElement::GetSecondaryColour() const
{
    return colour[1] & TILE_ELEMENT_COLOUR_MASK;
}

uint8_t RCT12LargeSceneryElement::GetBannerIndex() const
{
    return (type & 0xC0) | (((colour[0]) & ~TILE_ELEMENT_COLOUR_MASK) >> 2) | (((colour[1]) & ~TILE_ELEMENT_COLOUR_MASK) >> 5);
}

uint8_t RCT12WallElement::GetEntryIndex() const
{
    return entryIndex;
}

uint8_t RCT12WallElement::GetSlope() const
{
    return (type & TILE_ELEMENT_QUADRANT_MASK) >> 6;
}

colour_t RCT12WallElement::GetPrimaryColour() const
{
    return colour_1 & TILE_ELEMENT_COLOUR_MASK;
}

colour_t RCT12WallElement::GetSecondaryColour() const
{
    uint8_t secondaryColour = (colour_1 & ~TILE_ELEMENT_COLOUR_MASK) >> 5;
    secondaryColour |= (flags & 0x60) >> 2;
    return secondaryColour;
}

colour_t RCT12WallElement::GetTertiaryColour() const
{
    return colour_3 & TILE_ELEMENT_COLOUR_MASK;
}

uint8_t RCT12WallElement::GetAnimationFrame() const
{
    return (animation >> 3) & 0xF;
}

uint8_t RCT12WallElement::GetBannerIndex() const
{
    return banner_index;
}

bool RCT12WallElement::IsAcrossTrack() const
{
    return (animation & WALL_ANIMATION_FLAG_ACROSS_TRACK) != 0;
}

bool RCT12WallElement::AnimationIsBackwards() const
{
    return (animation & WALL_ANIMATION_FLAG_DIRECTION_BACKWARD) != 0;
}

uint32_t RCT12WallElement::GetRawRCT1WallTypeData() const
{
    return entryIndex | (colour_3 << 8) | (colour_1 << 16) | (animation << 24);
}

int32_t RCT12WallElement::GetRCT1WallType(int32_t edge) const
{
    uint8_t var_05 = colour_3;
    uint16_t var_06 = colour_1 | (animation << 8);

    int32_t typeA = (var_05 >> (edge * 2)) & 3;
    int32_t typeB = (var_06 >> (edge * 4)) & 0x0F;

    if (typeB != 0x0F)
    {
        return typeA | (typeB << 2);
    }
    else
    {
        return -1;
    }
}

colour_t RCT12WallElement::GetRCT1WallColour() const
{
    return ((type & 0xC0) >> 3) | ((entryIndex & 0xE0) >> 5);
}

uint8_t RCT12EntranceElement::GetEntranceType() const
{
    return entranceType;
}

uint8_t RCT12EntranceElement::GetRideIndex() const
{
    return rideIndex;
}

uint8_t RCT12EntranceElement::GetStationIndex() const
{
    return (index & RCT12_TRACK_ELEMENT_SEQUENCE_STATION_INDEX_MASK) >> 4;
}

uint8_t RCT12EntranceElement::GetSequenceIndex() const
{
    return index & 0xF;
}

uint8_t RCT12EntranceElement::GetPathType() const
{
    return pathType;
}

uint8_t RCT12BannerElement::GetIndex() const
{
    return index;
}

uint8_t RCT12BannerElement::GetPosition() const
{
    return position;
}

uint8_t RCT12BannerElement::GetAllowedEdges() const
{
    return flags & 0b00001111;
}

bool is_user_string_id(rct_string_id stringId)
{
    return stringId >= 0x8000 && stringId < 0x9000;
}

uint8_t RCT12TileElement::GetBannerIndex()
{
    rct_scenery_entry* sceneryEntry;

    switch (GetType())
    {
        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
            sceneryEntry = get_large_scenery_entry(AsLargeScenery()->GetEntryIndex());
            if (sceneryEntry->large_scenery.scrolling_mode == SCROLLING_MODE_NONE)
                return RCT12_BANNER_INDEX_NULL;

            return AsLargeScenery()->GetBannerIndex();
        case TILE_ELEMENT_TYPE_WALL:
            sceneryEntry = get_wall_entry(AsWall()->GetEntryIndex());
            if (sceneryEntry == nullptr || sceneryEntry->wall.scrolling_mode == SCROLLING_MODE_NONE)
                return RCT12_BANNER_INDEX_NULL;

            return AsWall()->GetBannerIndex();
        case TILE_ELEMENT_TYPE_BANNER:
            return AsBanner()->GetIndex();
        default:
            return RCT12_BANNER_INDEX_NULL;
    }
}

void RCT12TileElementBase::SetDirection(uint8_t direction)
{
    this->type &= ~TILE_ELEMENT_DIRECTION_MASK;
    this->type |= (direction & TILE_ELEMENT_DIRECTION_MASK);
}

void RCT12TileElement::ClearAs(uint8_t newType)
{
    type = newType;
    flags = 0;
    base_height = MINIMUM_LAND_HEIGHT;
    clearance_height = MINIMUM_LAND_HEIGHT;
    std::fill_n(pad_04, sizeof(pad_04), 0x00);
}

void RCT12LargeSceneryElement::SetEntryIndex(uint32_t newIndex)
{
    entryIndex &= ~RCT12_TILE_ELEMENT_LARGE_TYPE_MASK;
    entryIndex |= (newIndex & RCT12_TILE_ELEMENT_LARGE_TYPE_MASK);
}

void RCT12LargeSceneryElement::SetSequenceIndex(uint16_t sequence)
{
    entryIndex &= RCT12_TILE_ELEMENT_LARGE_TYPE_MASK;
    entryIndex |= (sequence << 10);
}

void RCT12LargeSceneryElement::SetPrimaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour[0] &= ~TILE_ELEMENT_COLOUR_MASK;
    colour[0] |= newColour;
}

void RCT12LargeSceneryElement::SetSecondaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour[1] &= ~TILE_ELEMENT_COLOUR_MASK;
    colour[1] |= newColour;
}

void RCT12LargeSceneryElement::SetBannerIndex(uint8_t newIndex)
{
    type |= newIndex & 0xC0;
    colour[0] |= (newIndex & 0x38) << 2;
    colour[1] |= (newIndex & 7) << 5;
}

void RCT12SurfaceElement::SetSurfaceStyle(uint32_t newStyle)
{
    // Bit 3 for terrain is stored in element.type bit 0
    if (newStyle & 8)
        type |= 1;
    else
        type &= ~1;

    // Bits 0, 1, 2 for terrain are stored in element.terrain bit 5, 6, 7
    terrain &= ~0xE0;
    terrain |= (newStyle & 7) << 5;
}

void RCT12SurfaceElement::SetEdgeStyle(uint32_t newStyle)
{
    // Bit 3 for terrain is stored in element.type bit 7
    if (newStyle & 8)
        type |= 128;
    else
        type &= ~128;

    // Bits 0, 1, 2 for terrain are stored in element.slope bit 5, 6, 7
    slope &= ~RCT12_TILE_ELEMENT_SURFACE_EDGE_STYLE_MASK;
    slope |= (newStyle & 7) << 5;
}

void RCT12SurfaceElement::SetWaterHeight(uint32_t newWaterHeight)
{
    newWaterHeight >>= 4;
    newWaterHeight &= 0x1F;
    terrain &= ~RCT12_TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK;
    terrain |= newWaterHeight;
}

void RCT12SurfaceElement::SetGrassLength(uint8_t newLength)
{
    grass_length = newLength;
}

void RCT12SurfaceElement::SetOwnership(uint8_t newOwnership)
{
    ownership &= ~TILE_ELEMENT_SURFACE_OWNERSHIP_MASK;
    ownership |= (newOwnership & TILE_ELEMENT_SURFACE_OWNERSHIP_MASK);
}

void RCT12SurfaceElement::SetParkFences(uint8_t newParkFences)
{
    ownership &= ~TILE_ELEMENT_SURFACE_PARK_FENCE_MASK;
    ownership |= (newParkFences & TILE_ELEMENT_SURFACE_PARK_FENCE_MASK);
}

void RCT12SurfaceElement::SetSlope(uint8_t newSlope)
{
    slope &= ~TILE_ELEMENT_SURFACE_SLOPE_MASK;
    slope |= (newSlope & TILE_ELEMENT_SURFACE_SLOPE_MASK);
}

void RCT12SurfaceElement::SetHasTrackThatNeedsWater(bool on)
{
    type &= ~SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER;
    if (on)
        type |= SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER;
}

void RCT12PathElement::SetPathEntryIndex(uint8_t newEntryIndex)
{
    entryIndex &= ~RCT12_FOOTPATH_PROPERTIES_TYPE_MASK;
    entryIndex |= (newEntryIndex << 4);
}

void RCT12PathElement::SetQueueBannerDirection(uint8_t direction)
{
    type &= ~FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK;
    type |= (direction << 6);
}

void RCT12PathElement::SetRideIndex(uint8_t newRideIndex)
{
    rideIndex = newRideIndex;
}

void RCT12PathElement::SetAdditionStatus(uint8_t newStatus)
{
    additionStatus = newStatus;
}

void RCT12PathElement::SetEdges(uint8_t newEdges)
{
    edges &= ~FOOTPATH_PROPERTIES_EDGES_EDGES_MASK;
    edges |= (newEdges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK);
}

void RCT12PathElement::SetCorners(uint8_t newCorners)
{
    edges &= ~FOOTPATH_PROPERTIES_EDGES_CORNERS_MASK;
    edges |= (newCorners << 4);
}

void RCT12PathElement::SetSloped(bool isSloped)
{
    entryIndex &= ~RCT12_FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
    if (isSloped)
        entryIndex |= RCT12_FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
}

void RCT12PathElement::SetSlopeDirection(uint8_t newSlope)
{
    entryIndex &= ~RCT12_FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
    entryIndex |= newSlope & RCT12_FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
}

void RCT12PathElement::SetStationIndex(uint8_t newStationIndex)
{
    additions &= ~RCT12_FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK;
    additions |= ((newStationIndex << 4) & RCT12_FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK);
}

void RCT12PathElement::SetWide(bool isWide)
{
    type &= ~FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE;
    if (isWide)
        type |= FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE;
}

void RCT12PathElement::SetIsQueue(bool isQueue)
{
    type &= ~FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE;
    if (isQueue)
        type |= FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE;
}

void RCT12PathElement::SetHasQueueBanner(bool hasQueueBanner)
{
    entryIndex &= ~RCT12_FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER;
    if (hasQueueBanner)
        entryIndex |= RCT12_FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER;
}

void RCT12PathElement::SetAddition(uint8_t newAddition)
{
    additions &= ~RCT12_FOOTPATH_PROPERTIES_ADDITIONS_TYPE_MASK;
    additions |= newAddition;
}

void RCT12PathElement::SetAdditionIsGhost(bool isGhost)
{
    additions &= ~RCT12_FOOTPATH_PROPERTIES_ADDITIONS_FLAG_GHOST;
    if (isGhost)
        additions |= RCT12_FOOTPATH_PROPERTIES_ADDITIONS_FLAG_GHOST;
}

bool RCT12PathElement::IsBroken() const
{
    return (flags & RCT12_TILE_ELEMENT_FLAG_BROKEN) != 0;
}

void RCT12PathElement::SetIsBroken(bool isBroken)
{
    if (isBroken)
    {
        flags |= RCT12_TILE_ELEMENT_FLAG_BROKEN;
    }
    else
    {
        flags &= ~RCT12_TILE_ELEMENT_FLAG_BROKEN;
    }
}

bool RCT12PathElement::IsBlockedByVehicle() const
{
    return (flags & RCT12_TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE) != 0;
}

void RCT12PathElement::SetIsBlockedByVehicle(bool isBlocked)
{
    if (isBlocked)
    {
        flags |= RCT12_TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE;
    }
    else
    {
        flags &= ~RCT12_TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE;
    }
}

void RCT12TrackElement::SetTrackType(uint8_t newType)
{
    trackType = newType;
}

void RCT12TrackElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    sequence &= ~RCT12_TRACK_ELEMENT_SEQUENCE_SEQUENCE_MASK;
    sequence |= (newSequenceIndex & RCT12_TRACK_ELEMENT_SEQUENCE_SEQUENCE_MASK);
}

void RCT12TrackElement::SetStationIndex(uint8_t newStationIndex)
{
    if (track_type_is_station(trackType) || trackType == TrackElemType::TowerBase)
    {
        sequence &= ~RCT12_TRACK_ELEMENT_SEQUENCE_STATION_INDEX_MASK;
        sequence |= (newStationIndex << 4);
    }
}

void RCT12TrackElement::SetRideIndex(uint8_t newRideIndex)
{
    rideIndex = newRideIndex;
}

void RCT12TrackElement::SetColourScheme(uint8_t newColourScheme)
{
    colour &= ~0x3;
    colour |= (newColourScheme & 0x3);
}

void RCT12TrackElement::SetHasCableLift(bool on)
{
    colour &= ~RCT12_TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
    if (on)
        colour |= RCT12_TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

void RCT12TrackElement::SetInverted(bool inverted)
{
    if (inverted)
    {
        colour |= RCT12_TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
    else
    {
        colour &= ~RCT12_TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
}

void RCT12TrackElement::SetBrakeBoosterSpeed(uint8_t speed)
{
    if (TrackTypeHasSpeedSetting(GetTrackType()))
    {
        sequence &= ~0b11110000;
        sequence |= ((speed >> 1) << 4);
    }
}

bool RCT12TrackElement::BlockBrakeClosed() const
{
    return (flags & RCT12_TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED) != 0;
}

void RCT12TrackElement::SetBlockBrakeClosed(bool isClosed)
{
    if (isClosed)
    {
        flags |= RCT12_TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED;
    }
    else
    {
        flags &= ~RCT12_TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED;
    }
}

void RCT12TrackElement::SetHasGreenLight(uint8_t greenLight)
{
    if (track_type_is_station(trackType))
    {
        sequence &= ~MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
        if (greenLight)
        {
            sequence |= MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
        }
    }
}

void RCT12TrackElement::SetHasChain(bool on)
{
    if (on)
    {
        type |= RCT12_TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
    }
    else
    {
        type &= ~RCT12_TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
    }
}

void RCT12TrackElement::SetSeatRotation(uint8_t newSeatRotation)
{
    colour &= 0x0F;
    colour |= (newSeatRotation << 4);
}

void RCT12TrackElement::SetMazeEntry(uint16_t newMazeEntry)
{
    mazeEntry = newMazeEntry;
}

void RCT12TrackElement::SetPhotoTimeout(uint8_t value)
{
    if (GetTrackType() == TrackElemType::OnRidePhoto)
    {
        sequence &= RCT12_TRACK_ELEMENT_SEQUENCE_SEQUENCE_MASK;
        sequence |= (value << 4);
    }
}

void RCT12SmallSceneryElement::SetEntryIndex(uint8_t newIndex)
{
    this->entryIndex = newIndex;
}

void RCT12SmallSceneryElement::SetAge(uint8_t newAge)
{
    this->age = newAge;
}

void RCT12SmallSceneryElement::SetPrimaryColour(colour_t colour)
{
    assert(colour <= 31);
    colour_1 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_1 |= colour;
}

void RCT12SmallSceneryElement::SetSecondaryColour(colour_t colour)
{
    assert(colour <= 31);
    colour_2 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_2 |= colour;
}

void RCT12SmallSceneryElement::SetSceneryQuadrant(uint8_t newQuadrant)
{
    type &= ~TILE_ELEMENT_QUADRANT_MASK;
    type |= (newQuadrant << 6);
}

void RCT12SmallSceneryElement::SetNeedsSupports()
{
    colour_1 |= MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS;
}

void RCT12WallElement::SetEntryIndex(uint8_t newIndex)
{
    entryIndex = newIndex;
}

void RCT12WallElement::SetBannerIndex(uint8_t newIndex)
{
    banner_index = newIndex;
}

void RCT12WallElement::SetAcrossTrack(bool acrossTrack)
{
    animation &= ~WALL_ANIMATION_FLAG_ACROSS_TRACK;
    if (acrossTrack)
        animation |= WALL_ANIMATION_FLAG_ACROSS_TRACK;
}

void RCT12WallElement::SetAnimationIsBackwards(bool isBackwards)
{
    animation &= ~WALL_ANIMATION_FLAG_DIRECTION_BACKWARD;
    if (isBackwards)
        animation |= WALL_ANIMATION_FLAG_DIRECTION_BACKWARD;
}
void RCT12WallElement::SetSlope(uint8_t newSlope)
{
    type &= ~TILE_ELEMENT_QUADRANT_MASK;
    type |= (newSlope << 6);
}

void RCT12WallElement::SetPrimaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour_1 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_1 |= newColour;
}

void RCT12WallElement::SetSecondaryColour(colour_t newColour)
{
    colour_1 &= TILE_ELEMENT_COLOUR_MASK;
    colour_1 |= (newColour & 0x7) << 5;
    flags &= ~0x60;
    flags |= (newColour & 0x18) << 2;
}

void RCT12WallElement::SetTertiaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour_3 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_3 |= newColour;
}

void RCT12WallElement::SetAnimationFrame(uint8_t frameNum)
{
    animation &= WALL_ANIMATION_FLAG_ALL_FLAGS;
    animation |= (frameNum & 0xF) << 3;
}

void RCT12EntranceElement::SetEntranceType(uint8_t newType)
{
    entranceType = newType;
}

void RCT12EntranceElement::SetRideIndex(uint8_t newRideIndex)
{
    rideIndex = newRideIndex;
}

void RCT12EntranceElement::SetPathType(uint8_t newPathType)
{
    pathType = newPathType;
}

void RCT12EntranceElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    index &= ~0xF;
    index |= (newSequenceIndex & 0xF);
}

void RCT12EntranceElement::SetStationIndex(uint8_t stationIndex)
{
    index &= ~RCT12_TRACK_ELEMENT_SEQUENCE_STATION_INDEX_MASK;
    index |= (stationIndex << 4);
}

void RCT12BannerElement::SetIndex(uint8_t newIndex)
{
    index = newIndex;
}

void RCT12BannerElement::SetPosition(uint8_t newPosition)
{
    position = newPosition;
}

void RCT12BannerElement::SetAllowedEdges(uint8_t newEdges)
{
    flags &= ~0b00001111;
    flags |= (newEdges & 0b00001111);
}

bool RCT12ResearchItem::IsInventedEndMarker() const
{
    return rawValue == RCT12_RESEARCHED_ITEMS_SEPARATOR;
}

bool RCT12ResearchItem::IsUninventedEndMarker() const
{
    return rawValue == RCT12_RESEARCHED_ITEMS_END;
}

bool RCT12ResearchItem::IsRandomEndMarker() const
{
    return rawValue == RCT12_RESEARCHED_ITEMS_END_2;
}

ObjectEntryIndex RCTEntryIndexToOpenRCT2EntryIndex(const RCT12ObjectEntryIndex index)
{
    if (index == RCT12_OBJECT_ENTRY_INDEX_NULL)
        return OBJECT_ENTRY_INDEX_NULL;

    return index;
}

RCT12ObjectEntryIndex OpenRCT2EntryIndexToRCTEntryIndex(const ObjectEntryIndex index)
{
    if (index == OBJECT_ENTRY_INDEX_NULL)
        return RCT12_OBJECT_ENTRY_INDEX_NULL;

    return index;
}
