/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Addresses.h"

#include <array>
#include <openrct2/Context.h>
#include <openrct2/config/Config.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/object/Object.h>
#include <openrct2/paint/tile_element/Paint.TileElement.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Station.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/world/Location.hpp>
#include <openrct2/world/Sprite.h>
#include <openrct2/world/Surface.h>

class StationObject;

#define gRideEntries RCT2_ADDRESS(0x009ACFA4, rct_ride_entry*)
#define gTileElementTilePointers RCT2_ADDRESS(0x013CE9A4, TileElement*)
rct_sprite* sprite_list = RCT2_ADDRESS(0x010E63BC, rct_sprite);

bool gCheatsEnableAllDrawableTrackPieces = false;

Ride gRideList[MAX_RIDES];
int16_t gMapBaseZ;
bool gTrackDesignSaveMode = false;
ride_id_t gTrackDesignSaveRideIndex = RIDE_ID_NULL;
uint8_t gClipHeight = 255;
CoordsXY gClipSelectionA = { 0, 0 };
CoordsXY gClipSelectionB = { MAXIMUM_TILE_START_XY, MAXIMUM_TILE_START_XY };
uint32_t gScenarioTicks;
uint8_t gCurrentRotation;

// clang-format off
constexpr const std::array<CoordsXY, 8> CoordsDirectionDelta = {
    CoordsXY{ -COORDS_XY_STEP, 0 },
    CoordsXY{               0, +COORDS_XY_STEP },
    CoordsXY{ +COORDS_XY_STEP, 0 },
    CoordsXY{               0, -COORDS_XY_STEP },
    CoordsXY{ -COORDS_XY_STEP, +COORDS_XY_STEP },
    CoordsXY{ +COORDS_XY_STEP, +COORDS_XY_STEP },
    CoordsXY{ +COORDS_XY_STEP, -COORDS_XY_STEP },
    CoordsXY{ -COORDS_XY_STEP, -COORDS_XY_STEP },
};

const TileCoordsXY TileDirectionDelta[] = {
    { -1, 0 },
    { 0, +1 },
    { +1, 0 },
    { 0, -1 },
    { -1, +1 },
    { +1, +1 },
    { +1, -1 },
    { -1, -1 },
};
// clang-format on

uint8_t get_current_rotation()
{
    return gCurrentRotation & 3;
}

int object_entry_group_counts[] = {
    128, // ObjectType::Ride
    252, // ObjectType::SmallScenery
    128, // ObjectType::LargeScenery
    128, // ObjectType::Walls
    32,  // ObjectType::Banners
    16,  // ObjectType::Paths
    15,  // ObjectType::PathBits
    19,  // ObjectType::SceneryGroup
    1,   // ObjectType::ParkEntrance
    1,   // ObjectType::Water
    1,   // ObjectType::ScenarioText
    0,   // ObjectType::TerrainSurface
    0,   // ObjectType::TerrainEdge
    0,   // ObjectType::Station
    0,   // ObjectType::Music
    0,   // ObjectType::FootpathSurface
    0,   // ObjectType::FootpathRailings
};
static_assert(std::size(object_entry_group_counts) == EnumValue(ObjectType::Count));

GeneralConfiguration gConfigGeneral;
uint16_t gMapSelectFlags;
uint16_t gMapSelectType;
CoordsXY gMapSelectPositionA;
CoordsXY gMapSelectPositionB;
CoordsXYZ gMapSelectArrowPosition;
uint8_t gMapSelectArrowDirection;

void entrance_paint(paint_session* session, uint8_t direction, int height, const TileElement* tile_element)
{
}
void banner_paint(paint_session* session, uint8_t direction, int height, const TileElement* tile_element)
{
}
void surface_paint(paint_session* session, uint8_t direction, uint16_t height, const TileElement* tileElement)
{
}
void path_paint(paint_session* session, uint16_t height, const TileElement* tileElement)
{
}
void scenery_paint(paint_session* session, uint8_t direction, int height, const TileElement* tileElement)
{
}
void fence_paint(paint_session* session, uint8_t direction, int height, const TileElement* tileElement)
{
}
void large_scenery_paint(paint_session* session, uint8_t direction, uint16_t height, const TileElement* tileElement)
{
}

Ride* get_ride(ride_id_t index)
{
    if (index >= RCT12_MAX_RIDES_IN_PARK)
    {
        log_error("invalid index %d for ride", index);
        return nullptr;
    }
    return &gRideList[index];
}

rct_ride_entry* get_ride_entry(ObjectEntryIndex index)
{
    if (index >= object_entry_group_counts[static_cast<int>(ObjectType::Ride)])
    {
        log_error("invalid index %d for ride type", index);
        return nullptr;
    }

    return gRideEntries[index];
}

rct_ride_entry* Ride::GetRideEntry() const
{
    rct_ride_entry* rideEntry = get_ride_entry(subtype);
    if (rideEntry == nullptr)
    {
        log_error("Invalid ride subtype for ride");
    }
    return rideEntry;
}

template<> bool SpriteBase::Is<SpriteBase>() const
{
    return true;
}

template<> bool SpriteBase::Is<Peep>() const
{
    return Type == EntityType::Guest || Type == EntityType::Staff;
}

template<> bool SpriteBase::Is<Guest>() const
{
    return Type == EntityType::Guest;
}

template<> bool SpriteBase::Is<Vehicle>() const
{
    return Type == EntityType::Vehicle;
}

SpriteBase* get_sprite(size_t sprite_idx)
{
    assert(sprite_idx < MAX_ENTITIES);
    return reinterpret_cast<SpriteBase*>(&sprite_list[sprite_idx]);
}

bool TileElementBase::IsLastForTile() const
{
    return (this->Flags & TILE_ELEMENT_FLAG_LAST_TILE) != 0;
}

void TileElementBase::SetLastForTile(bool on)
{
    if (on)
        Flags |= TILE_ELEMENT_FLAG_LAST_TILE;
    else
        Flags &= ~TILE_ELEMENT_FLAG_LAST_TILE;
}

uint8_t TileElementBase::GetType() const
{
    return this->type & TILE_ELEMENT_TYPE_MASK;
}

bool TileElementBase::IsGhost() const
{
    return (this->Flags & TILE_ELEMENT_FLAG_GHOST) != 0;
}

bool TrackElement::BlockBrakeClosed() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_BLOCK_BRAKE_CLOSED) != 0;
}

TileElement* map_get_first_element_at(const CoordsXY& elementPos)
{
    if (elementPos.x < 0 || elementPos.y < 0 || elementPos.x > 255 || elementPos.y > 255)
    {
        log_error("Trying to access element outside of range");
        return nullptr;
    }
    auto tileElementPos = TileCoordsXY{ elementPos };
    return gTileElementTilePointers[tileElementPos.x + tileElementPos.y * 256];
}

int32_t get_height_marker_offset()
{
    return 0;
}

bool is_csg_loaded()
{
    return false;
}

uint8_t TrackElement::GetSeatRotation() const
{
    const auto* ride = get_ride(GetRideIndex());
    if (ride != nullptr && ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_LANDSCAPE_DOORS))
        return DEFAULT_SEAT_ROTATION;

    return ColourScheme >> 4;
}

void TrackElement::SetSeatRotation(uint8_t newSeatRotation)
{
    ColourScheme &= ~TRACK_ELEMENT_COLOUR_SEAT_ROTATION_MASK;
    ColourScheme |= (newSeatRotation << 4);
}

bool TrackElement::IsTakingPhoto() const
{
    return OnridePhotoBits != 0;
}

void TrackElement::SetPhotoTimeout()
{
    OnridePhotoBits = 3;
}

void TrackElement::SetPhotoTimeout(uint8_t value)
{
    OnridePhotoBits = value;
}

uint8_t TrackElement::GetPhotoTimeout() const
{
    return OnridePhotoBits;
}

void TrackElement::DecrementPhotoTimeout()
{
    OnridePhotoBits = std::max(0, OnridePhotoBits - 1);
}

uint16_t TrackElement::GetMazeEntry() const
{
    return MazeEntry;
}

void TrackElement::SetMazeEntry(uint16_t newMazeEntry)
{
    MazeEntry = newMazeEntry;
}

void TrackElement::MazeEntryAdd(uint16_t addVal)
{
    MazeEntry |= addVal;
}

void TrackElement::MazeEntrySubtract(uint16_t subVal)
{
    MazeEntry &= ~subVal;
}

track_type_t TrackElement::GetTrackType() const
{
    return TrackType;
}

void TrackElement::SetTrackType(track_type_t newType)
{
    TrackType = newType;
}

uint8_t TrackElement::GetSequenceIndex() const
{
    return Sequence;
}

void TrackElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    Sequence = newSequenceIndex;
}

uint8_t TrackElement::GetStationIndex() const
{
    return StationIndex;
}

void TrackElement::SetStationIndex(uint8_t newStationIndex)
{
    StationIndex = newStationIndex;
}

uint8_t TrackElement::GetDoorAState() const
{
    return (ColourScheme & TRACK_ELEMENT_COLOUR_DOOR_A_MASK) >> 2;
}

uint8_t TrackElement::GetDoorBState() const
{
    return (ColourScheme & TRACK_ELEMENT_COLOUR_DOOR_B_MASK) >> 5;
}

ride_id_t TrackElement::GetRideIndex() const
{
    return RideIndex;
}

void TrackElement::SetRideIndex(ride_id_t newRideIndex)
{
    RideIndex = newRideIndex;
}

uint8_t TrackElement::GetColourScheme() const
{
    return ColourScheme & TRACK_ELEMENT_COLOUR_SCHEME_MASK;
}

void TrackElement::SetColourScheme(uint8_t newColourScheme)
{
    ColourScheme &= ~TRACK_ELEMENT_COLOUR_SCHEME_MASK;
    ColourScheme |= (newColourScheme & TRACK_ELEMENT_COLOUR_SCHEME_MASK);
}

bool TrackElement::HasCableLift() const
{
    return Flags2 & TRACK_ELEMENT_FLAGS2_CABLE_LIFT;
}

void TrackElement::SetHasCableLift(bool on)
{
    Flags2 &= ~TRACK_ELEMENT_FLAGS2_CABLE_LIFT;
    if (on)
        Flags2 |= TRACK_ELEMENT_FLAGS2_CABLE_LIFT;
}

bool TrackElement::IsInverted() const
{
    return Flags2 & TRACK_ELEMENT_FLAGS2_INVERTED;
}

void TrackElement::SetInverted(bool inverted)
{
    if (inverted)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_INVERTED;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_INVERTED;
    }
}

uint8_t TrackElement::GetBrakeBoosterSpeed() const
{
    return BrakeBoosterSpeed << 1;
}

void TrackElement::SetBrakeBoosterSpeed(uint8_t speed)
{
    BrakeBoosterSpeed = (speed >> 1);
}

bool TrackElement::HasGreenLight() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT) != 0;
}

void TrackElement::SetHasGreenLight(bool on)
{
    Flags2 &= ~TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT;
    if (on)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT;
    }
}

bool TrackElement::HasChain() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_CHAIN_LIFT) != 0;
}

void TrackElement::SetHasChain(bool on)
{
    if (on)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_CHAIN_LIFT;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_CHAIN_LIFT;
    }
}

TileCoordsXYZD ride_get_entrance_location(const Ride* ride, const StationIndex stationIndex)
{
    return ride->stations[stationIndex].Entrance;
}

TileCoordsXYZD ride_get_exit_location(const Ride* ride, const StationIndex stationIndex)
{
    return ride->stations[stationIndex].Exit;
}

void TileElementBase::SetType(uint8_t newType)
{
    this->type &= ~TILE_ELEMENT_TYPE_MASK;
    this->type |= (newType & TILE_ELEMENT_TYPE_MASK);
}

uint8_t TileElementBase::GetDirection() const
{
    return this->type & TILE_ELEMENT_DIRECTION_MASK;
}

uint8_t TileElementBase::GetDirectionWithOffset(uint8_t offset) const
{
    return ((this->type & TILE_ELEMENT_DIRECTION_MASK) + offset) & TILE_ELEMENT_DIRECTION_MASK;
}

uint8_t SurfaceElement::GetSlope() const
{
    return Slope;
}

int32_t SurfaceElement::GetWaterHeight() const
{
    return WaterHeight * 16;
}

bool TrackElement::IsHighlighted() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_HIGHLIGHT);
}

uint8_t PathElement::GetEdges() const
{
    return EdgesAndCorners & 0xF;
}

StationObject* ride_get_station_object(const Ride* ride)
{
    return nullptr;
}

Ride* Vehicle::GetRide() const
{
    return get_ride(ride);
}

bool Vehicle::IsGhost() const
{
    auto r = GetRide();
    return r != nullptr && r->status == RideStatus::Simulating;
}

uint8_t TileElementBase::GetOccupiedQuadrants() const
{
    return Flags & TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK;
}

void TileElementBase::SetOccupiedQuadrants(uint8_t quadrants)
{
    Flags &= ~TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK;
    Flags |= (quadrants & TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK);
}

int32_t TileElementBase::GetBaseZ() const
{
    return base_height * COORDS_Z_STEP;
}

void TileElementBase::SetBaseZ(int32_t newZ)
{
    base_height = (newZ / COORDS_Z_STEP);
}

int32_t TileElementBase::GetClearanceZ() const
{
    return clearance_height * COORDS_Z_STEP;
}

void TileElementBase::SetClearanceZ(int32_t newZ)
{
    clearance_height = (newZ / COORDS_Z_STEP);
}

int32_t RideStation::GetBaseZ() const
{
    return Height * COORDS_Z_STEP;
}

void RideStation::SetBaseZ(int32_t newZ)
{
    Height = newZ / COORDS_Z_STEP;
}

CoordsXYZ RideStation::GetStart() const
{
    TileCoordsXYZ stationTileCoords{ Start.x, Start.y, Height };
    return stationTileCoords.ToCoordsXYZ();
}

bool TrackElement::IsStation() const
{
    return track_type_is_station(GetTrackType());
}

bool track_type_is_station(track_type_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return true;
        default:
            return false;
    }
}

void ride_ratings_calculate_spiral_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_stand_up_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_suspended_swinging_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_inverted_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_junior_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_miniature_railway([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_monorail([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_mini_suspended_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_boat_hire([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_wooden_wild_mouse([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_steeplechase([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_car_ride([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_launched_freefall([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_bobsleigh_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_observation_tower([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_looping_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_dinghy_slide([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_mine_train_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_chairlift([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_corkscrew_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_maze([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_spiral_slide([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_go_karts([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_log_flume([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_river_rapids([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_dodgems([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_swinging_ship([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_inverter_ship([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_food_stall([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_shop([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_merry_go_round([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_information_kiosk([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_toilets([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_ferris_wheel([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_motion_simulator([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_3d_cinema([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_top_spin([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_space_rings([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_reverse_freefall_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_lift([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_vertical_drop_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_cash_machine([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_twist([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_haunted_house([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_first_aid([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_circus([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_ghost_train([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_twister_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_wooden_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_side_friction_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_wild_mouse([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_multi_dimension_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_flying_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_virginia_reel([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_splash_boats([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_mini_helicopters([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_lay_down_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_suspended_monorail([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_reverser_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_heartline_twister_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_mini_golf([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_giga_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_roto_drop([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_flying_saucers([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_crooked_house([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_monorail_cycles([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_compact_inverted_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_water_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_air_powered_vertical_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_inverted_hairpin_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_magic_carpet([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_submarine_ride([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_river_rafts([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_enterprise([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_inverted_impulse_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_mini_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_mine_ride([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_lim_launched_roller_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_drink_stall([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_hybrid_coaster([[maybe_unused]] Ride* ride)
{
}

void ride_ratings_calculate_single_rail_roller_coaster([[maybe_unused]] Ride* ride)
{
}

const RideTypeDescriptor& Ride::GetRideTypeDescriptor() const
{
    return ::GetRideTypeDescriptor(type);
}

uint8_t TileElementBase::GetOwner() const
{
    return owner & OWNER_MASK;
}

void TileElementBase::SetOwner(uint8_t newOwner)
{
    owner &= ~OWNER_MASK;
    owner |= (newOwner & OWNER_MASK);
}

namespace OpenRCT2
{
    IContext* GetContext()
    {
        return nullptr;
    }
} // namespace OpenRCT2

ScreenCoordsXY translate_3d_to_2d_with_z(int32_t rotation, const CoordsXYZ& pos)
{
    auto rotated = pos.Rotate(rotation);
    // Use right shift to avoid issues like #9301
    return ScreenCoordsXY{ rotated.y - rotated.x, ((rotated.x + rotated.y) >> 1) - pos.z };
}
