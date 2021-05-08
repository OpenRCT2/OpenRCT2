/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/config/Config.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/network/network.h>
#include <openrct2/object/Object.h>
#include <openrct2/ride/CableLift.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Station.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackPaint.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/world/Location.hpp>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/SmallScenery.h>
#include <openrct2/world/Sprite.h>
#include <openrct2/world/Surface.h>

// Fields actually used:
/*
rct_ride_entry:
vehicles
flags & RIDE_ENTRY_FLAG_PLAY_DEPART_SOUND
flags & RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_1
flags & RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_2
flags & RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_1
flags & RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_2
flags & RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND
flags & RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND_SLIDE
flags & RIDE_ENTRY_FLAG_DISABLE_COLLISION_CRASHES

vehicleEntry->sound_range
vehicleEntry->friction_sound_id
vehicleEntry->spinning_inertia
vehicleEntry->spinning_friction
vehicleEntry->animation
vehicleEntry->log_flume_reverser_vehicle_type

rideEntry->vehicles[0].flags & VEHICLE_ENTRY_FLAG_25
rideEntry->vehicles[0].flags & VEHICLE_ENTRY_FLAG_POWERED

vehicleEntry->animation == VEHICLE_ENTRY_ANIMATION_OBSERVATION_TOWER

vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SPINNING
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SWINGING
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_POWERED
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_LIFT
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_WATER_RIDE
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_DODGEM_INUSE_LIGHTS
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_GO_KART
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_WATER_RIDE
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_RIDERS_SCREAM
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_POWERED_RIDE_UNRESTRICTED_GRAVITY
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_NO_UPSTOP_WHEELS
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_NO_UPSTOP_BOBSLEIGH
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_CHAIRLIFT
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_21
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_25
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SLIDE_SWING
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_MINI_GOLF
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_VEHICLE_ANIMATION
vehicleEntry->flags & VEHICLE_ENTRY_FLAG_RIDER_ANIMATION
*/

static rct_ride_entry _test_ride_entries[128]{};

class StationObject;
class Intent;

TileElement* gTileElementTilePointers[MAX_TILE_TILE_ELEMENT_POINTERS]{};

bool gCheatsEnableAllDrawableTrackPieces = false;

static Ride _rideList[MAX_RIDES]{};
uint32_t gScenarioTicks{};
uint8_t gCurrentRotation{};
uint16_t gSavedAge{};
uint32_t gCurrentTicks{};
NotificationConfiguration gConfigNotifications{};
bool gCheatsDisableLittering = false;
int16_t gParkRatingCasualtyPenalty{};
rct_string_id gGameCommandErrorText{};

// clang-format off
const size_t mini_golf_peep_animation_lengths[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

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
// clang-format on

uint8_t get_current_rotation()
{
    return gCurrentRotation & 3;
}

int object_entry_group_counts[] = {
    128, // rides
    252, // small scenery
    128, // large scenery
    128, // walls
    32,  // banners
    16,  // paths
    15,  // path bits
    19,  // scenery sets
    1,   // park entrance
    1,   // water
    1    // scenario text
};

bool map_is_location_valid(const CoordsXY& coords)
{
    const bool is_x_valid = coords.x < MAXIMUM_MAP_SIZE_BIG && coords.x >= 0;
    const bool is_y_valid = coords.y < MAXIMUM_MAP_SIZE_BIG && coords.y >= 0;
    return is_x_valid && is_y_valid;
}

TileElement* map_get_first_element_at(const CoordsXY& elementPos)
{
    if (!map_is_location_valid(elementPos))
    {
        log_verbose("Trying to access element outside of range");
        return nullptr;
    }
    auto tileElementPos = TileCoordsXY{ elementPos };
    return gTileElementTilePointers[tileElementPos.x + tileElementPos.y * MAXIMUM_MAP_SIZE_TECHNICAL];
}
SurfaceElement* map_get_surface_element_at(const CoordsXY& coords)
{
    return nullptr;
}

bool track_block_get_next_from_zero(
    const CoordsXYZ& startPos, Ride* ride, uint8_t direction_start, CoordsXYE* output, int32_t* z, int32_t* direction,
    bool isGhost)
{
    // TODO(janisozaur): do something here
    return false;
}

random_engine_t::result_type scenario_rand()
{
    // TODO(janisozaur): do something here
    return {};
}

int16_t tile_element_height(const CoordsXY& loc)
{
    // TODO(janisozaur): do something here
    return 0;
}

int16_t tile_element_water_height(const CoordsXY& loc)
{
    // TODO(janisozaur): do something here
    return 0;
}

bool tile_element_is_underground(TileElement* tileElement)
{
    // TODO(janisozaur): do something here
    return false;
}

void tile_element_remove(TileElement* tileElement)
{
    // Don't expect to run into element removal in this test
    assert(false);
}

TrackElement* map_get_track_element_at(const CoordsXYZ& trackPos)
{
    // TODO(janisozaur): do something here
    return nullptr;
}

PathElement* map_get_path_element_at(const TileCoordsXYZ& loc)
{
    // TODO(janisozaur): use the real function here?
    return nullptr;
}
TileElement* map_get_track_element_at_of_type(const CoordsXYZ& trackPos, track_type_t trackType)
{
    // TODO(janisozaur): use the real function here?
    return nullptr;
}
TileElement* map_get_track_element_at_of_type_seq(const CoordsXYZ& trackPos, track_type_t trackType, int32_t sequence)
{
    // TODO(janisozaur): use the real function here?
    return nullptr;
}

bool track_block_get_previous(const CoordsXYE& trackPos, track_begin_end* outTrackBeginEnd)
{
    // TODO(janisozaur): do something here
    return false;
}
void map_animation_create(int32_t type, const CoordsXYZ& loc)
{
    // Probably overkill
    assert(false);
}

bool track_block_get_next(CoordsXYE* input, CoordsXYE* output, int32_t* z, int32_t* direction)
{
    // TODO(janisozaur): do something here
    return false;
}

TileElement* map_get_track_element_at_from_ride(const CoordsXYZ& trackPos, ride_id_t rideIndex)
{
    // TODO(janisozaur): do something here
    return nullptr;
}

void map_invalidate_element(const CoordsXY& elementPos, TileElement* tileElement)
{
    // Probably overkill
    assert(false);
}

ScreenCoordsXY translate_3d_to_2d_with_z(int32_t rotation, const CoordsXYZ& pos)
{
    auto rotated = pos.Rotate(rotation);
    // Use right shift to avoid issues like #9301
    return ScreenCoordsXY{ rotated.y - rotated.x, ((rotated.x + rotated.y) >> 1) - pos.z };
}

bool map_is_location_owned(const CoordsXYZ& loc)
{
    // Probably overkill
    assert(false);
    return true;
}

TileElement* map_get_track_element_at_with_direction_from_ride(const CoordsXYZD& trackPos, ride_id_t rideIndex)
{
    // Probably overkill
    assert(false);
    return nullptr;
}

void map_invalidate_tile_zoom1(const CoordsXYRangedZ& tilePos)
{
    // Probably overkill
    assert(false);
}

Ride* get_ride(ride_id_t index)
{
    if (index >= RCT12_MAX_RIDES_IN_PARK)
    {
        log_error("invalid index %d for ride", index);
        return nullptr;
    }
    return &_rideList[index];
}

rct_ride_entry* get_ride_entry(ObjectEntryIndex index)
{
    if (index >= object_entry_group_counts[static_cast<int>(ObjectType::Ride)])
    {
        log_error("invalid index %d for ride type", index);
        return nullptr;
    }

    return &_test_ride_entries[index];
}

bool ride_type_has_flag(int32_t rideType, uint64_t flag)
{
    if (rideType < static_cast<int32_t>(std::size(RideTypeDescriptors)))
    {
        return (RideTypeDescriptors[rideType].Flags & flag) != 0;
    }
    else
    {
        Guard::Assert(false);
        return false;
    }
}

int32_t SurfaceElement::GetWaterHeight() const
{
    // Probably overkill
    assert(false);
    return WaterHeight * 16;
}

StationObject* ride_get_station_object(const Ride* ride)
{
    // Probably overkill
    assert(false);
    return nullptr;
}

#define RIDE_RATING_FN(name)                                                                                                   \
    void ride_ratings_calculate_##name(Ride*)                                                                                  \
    {                                                                                                                          \
        assert(false);                                                                                                         \
    }

RIDE_RATING_FN(spiral_roller_coaster)
RIDE_RATING_FN(stand_up_roller_coaster)
RIDE_RATING_FN(suspended_swinging_coaster)
RIDE_RATING_FN(inverted_roller_coaster)
RIDE_RATING_FN(junior_roller_coaster)
RIDE_RATING_FN(miniature_railway)
RIDE_RATING_FN(monorail)
RIDE_RATING_FN(mini_suspended_coaster)
RIDE_RATING_FN(boat_hire)
RIDE_RATING_FN(wooden_wild_mouse)
RIDE_RATING_FN(steeplechase)
RIDE_RATING_FN(car_ride)
RIDE_RATING_FN(launched_freefall)
RIDE_RATING_FN(bobsleigh_coaster)
RIDE_RATING_FN(observation_tower)
RIDE_RATING_FN(looping_roller_coaster)
RIDE_RATING_FN(dinghy_slide)
RIDE_RATING_FN(mine_train_coaster)
RIDE_RATING_FN(chairlift)
RIDE_RATING_FN(corkscrew_roller_coaster)
RIDE_RATING_FN(maze)
RIDE_RATING_FN(spiral_slide)
RIDE_RATING_FN(go_karts)
RIDE_RATING_FN(log_flume)
RIDE_RATING_FN(river_rapids)
RIDE_RATING_FN(dodgems)
RIDE_RATING_FN(swinging_ship)
RIDE_RATING_FN(inverter_ship)
RIDE_RATING_FN(food_stall)
RIDE_RATING_FN(shop)
RIDE_RATING_FN(merry_go_round)
RIDE_RATING_FN(information_kiosk)
RIDE_RATING_FN(toilets)
RIDE_RATING_FN(ferris_wheel)
RIDE_RATING_FN(motion_simulator)
RIDE_RATING_FN(3d_cinema)
RIDE_RATING_FN(top_spin)
RIDE_RATING_FN(space_rings)
RIDE_RATING_FN(reverse_freefall_coaster)
RIDE_RATING_FN(lift)
RIDE_RATING_FN(vertical_drop_roller_coaster)
RIDE_RATING_FN(cash_machine)
RIDE_RATING_FN(twist)
RIDE_RATING_FN(haunted_house)
RIDE_RATING_FN(first_aid)
RIDE_RATING_FN(circus)
RIDE_RATING_FN(ghost_train)
RIDE_RATING_FN(twister_roller_coaster)
RIDE_RATING_FN(wooden_roller_coaster)
RIDE_RATING_FN(side_friction_roller_coaster)
RIDE_RATING_FN(wild_mouse)
RIDE_RATING_FN(multi_dimension_roller_coaster)
RIDE_RATING_FN(flying_roller_coaster)
RIDE_RATING_FN(virginia_reel)
RIDE_RATING_FN(splash_boats)
RIDE_RATING_FN(mini_helicopters)
RIDE_RATING_FN(lay_down_roller_coaster)
RIDE_RATING_FN(suspended_monorail)
RIDE_RATING_FN(reverser_roller_coaster)
RIDE_RATING_FN(heartline_twister_coaster)
RIDE_RATING_FN(mini_golf)
RIDE_RATING_FN(giga_coaster)
RIDE_RATING_FN(roto_drop)
RIDE_RATING_FN(flying_saucers)
RIDE_RATING_FN(crooked_house)
RIDE_RATING_FN(monorail_cycles)
RIDE_RATING_FN(compact_inverted_coaster)
RIDE_RATING_FN(water_coaster)
RIDE_RATING_FN(air_powered_vertical_coaster)
RIDE_RATING_FN(inverted_hairpin_coaster)
RIDE_RATING_FN(magic_carpet)
RIDE_RATING_FN(submarine_ride)
RIDE_RATING_FN(river_rafts)
RIDE_RATING_FN(enterprise)
RIDE_RATING_FN(inverted_impulse_coaster)
RIDE_RATING_FN(mini_roller_coaster)
RIDE_RATING_FN(mine_ride)
RIDE_RATING_FN(lim_launched_roller_coaster)
RIDE_RATING_FN(drink_stall)
RIDE_RATING_FN(hybrid_coaster)
RIDE_RATING_FN(single_rail_roller_coaster)

void ride_breakdown_add_news_item(Ride* ride)
{
    // Probably overkill
    assert(false);
}
void window_invalidate_by_number(rct_windowclass cls, rct_windownumber number)
{
    // Probably overkill
    assert(false);
}
void increment_turn_count_1_element(Ride* ride, uint8_t type)
{
    uint16_t* turn_count;
    switch (type)
    {
        case 0:
            turn_count = &ride->turn_count_default;
            break;
        case 1:
            turn_count = &ride->turn_count_banked;
            break;
        case 2:
            turn_count = &ride->turn_count_sloped;
            break;
        default:
            return;
    }
    uint16_t value = (*turn_count & TURN_MASK_1_ELEMENT) + 1;
    *turn_count &= ~TURN_MASK_1_ELEMENT;

    if (value > TURN_MASK_1_ELEMENT)
        value = TURN_MASK_1_ELEMENT;
    *turn_count |= value;
}

void increment_turn_count_2_elements(Ride* ride, uint8_t type)
{
    uint16_t* turn_count;
    switch (type)
    {
        case 0:
            turn_count = &ride->turn_count_default;
            break;
        case 1:
            turn_count = &ride->turn_count_banked;
            break;
        case 2:
            turn_count = &ride->turn_count_sloped;
            break;
        default:
            return;
    }
    uint16_t value = (*turn_count & TURN_MASK_2_ELEMENTS) + 0x20;
    *turn_count &= ~TURN_MASK_2_ELEMENTS;

    if (value > TURN_MASK_2_ELEMENTS)
        value = TURN_MASK_2_ELEMENTS;
    *turn_count |= value;
}

void increment_turn_count_3_elements(Ride* ride, uint8_t type)
{
    uint16_t* turn_count;
    switch (type)
    {
        case 0:
            turn_count = &ride->turn_count_default;
            break;
        case 1:
            turn_count = &ride->turn_count_banked;
            break;
        case 2:
            turn_count = &ride->turn_count_sloped;
            break;
        default:
            return;
    }
    uint16_t value = (*turn_count & TURN_MASK_3_ELEMENTS) + 0x100;
    *turn_count &= ~TURN_MASK_3_ELEMENTS;

    if (value > TURN_MASK_3_ELEMENTS)
        value = TURN_MASK_3_ELEMENTS;
    *turn_count |= value;
}

void increment_turn_count_4_plus_elements(Ride* ride, uint8_t type)
{
    uint16_t* turn_count;
    switch (type)
    {
        case 0:
        case 1:
            // Just in case fallback to 3 element turn
            increment_turn_count_3_elements(ride, type);
            return;
        case 2:
            turn_count = &ride->turn_count_sloped;
            break;
        default:
            return;
    }
    uint16_t value = (*turn_count & TURN_MASK_4_PLUS_ELEMENTS) + 0x800;
    *turn_count &= ~TURN_MASK_4_PLUS_ELEMENTS;

    if (value > TURN_MASK_4_PLUS_ELEMENTS)
        value = TURN_MASK_4_PLUS_ELEMENTS;
    *turn_count |= value;
}
uint8_t ride_get_helix_sections(Ride* ride)
{
    // TODO(janisozaur): needs to be hit
    // Helix sections stored in the low 5 bits.
    return ride->special_track_elements & 0x1F;
}

rct_scenery_entry* SmallSceneryElement::GetEntry() const
{
    // TODO: janisozaur: add something here
    return nullptr;
}

bool scenery_small_entry_has_flag(const rct_scenery_entry* sceneryEntry, uint32_t flags)
{
    return static_cast<bool>(sceneryEntry->small_scenery.flags & flags);
}

const RideTypeDescriptor& Ride::GetRideTypeDescriptor() const
{
    return RideTypeDescriptors[type];
}

bool Ride::SupportsStatus(int32_t s) const
{
    const auto& rtd = GetRideTypeDescriptor();

    switch (s)
    {
        case RIDE_STATUS_CLOSED:
        case RIDE_STATUS_OPEN:
            return true;
        case RIDE_STATUS_SIMULATING:
            return (!rtd.HasFlag(RIDE_TYPE_FLAG_NO_TEST_MODE) && rtd.HasFlag(RIDE_TYPE_FLAG_HAS_TRACK));
        case RIDE_STATUS_TESTING:
            return !rtd.HasFlag(RIDE_TYPE_FLAG_NO_TEST_MODE);
        default:
            return false;
    }
}

void OpenRCT2::Audio::Play3D(SoundId soundId, const CoordsXYZ& loc)
{
    // TODO(janisozaur): use this for observing side effects, e.g. calls to Vehicle::Claxon
}

bool Ride::IsBlockSectioned() const
{
    return false;
}
static void dummy_paint_function(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    assert(false);
}

#define TRACK_PAINT_FN_GETTER(name)                                                                                            \
    TRACK_PAINT_FUNCTION get_track_paint_function_##name(int32_t)                                                              \
    {                                                                                                                          \
        return dummy_paint_function;                                                                                           \
    }

TRACK_PAINT_FN_GETTER(stand_up_rc)
TRACK_PAINT_FN_GETTER(suspended_swinging_rc)
TRACK_PAINT_FN_GETTER(inverted_rc)
TRACK_PAINT_FN_GETTER(junior_rc)
TRACK_PAINT_FN_GETTER(monorail)
TRACK_PAINT_FN_GETTER(mini_suspended_rc)
TRACK_PAINT_FN_GETTER(miniature_railway)
TRACK_PAINT_FN_GETTER(boat_hire)
TRACK_PAINT_FN_GETTER(wooden_wild_mouse)
TRACK_PAINT_FN_GETTER(steeplechase)
TRACK_PAINT_FN_GETTER(car_ride)
TRACK_PAINT_FN_GETTER(launched_freefall)
TRACK_PAINT_FN_GETTER(bobsleigh_rc)
TRACK_PAINT_FN_GETTER(observation_tower)
TRACK_PAINT_FN_GETTER(looping_rc)
TRACK_PAINT_FN_GETTER(dinghy_slide)
TRACK_PAINT_FN_GETTER(mine_train_rc)
TRACK_PAINT_FN_GETTER(chairlift)
TRACK_PAINT_FN_GETTER(corkscrew_rc)
TRACK_PAINT_FN_GETTER(maze)
TRACK_PAINT_FN_GETTER(spiral_slide)
TRACK_PAINT_FN_GETTER(go_karts)
TRACK_PAINT_FN_GETTER(log_flume)
TRACK_PAINT_FN_GETTER(river_rapids)
TRACK_PAINT_FN_GETTER(dodgems)
TRACK_PAINT_FN_GETTER(swinging_ship)
TRACK_PAINT_FN_GETTER(swinging_inverter_ship)
TRACK_PAINT_FN_GETTER(ferris_wheel)
TRACK_PAINT_FN_GETTER(motionsimulator)
TRACK_PAINT_FN_GETTER(3d_cinema)
TRACK_PAINT_FN_GETTER(topspin)
TRACK_PAINT_FN_GETTER(space_rings)
TRACK_PAINT_FN_GETTER(reverse_freefall_rc)
TRACK_PAINT_FN_GETTER(lift)
TRACK_PAINT_FN_GETTER(vertical_drop_rc)
TRACK_PAINT_FN_GETTER(shop)
TRACK_PAINT_FN_GETTER(merry_go_round)
TRACK_PAINT_FN_GETTER(facility)
TRACK_PAINT_FN_GETTER(twist)
TRACK_PAINT_FN_GETTER(haunted_house)
TRACK_PAINT_FN_GETTER(circus)
TRACK_PAINT_FN_GETTER(ghost_train)
TRACK_PAINT_FN_GETTER(twister_rc)
TRACK_PAINT_FN_GETTER(side_friction_rc)
TRACK_PAINT_FN_GETTER(wooden_rc)
TRACK_PAINT_FN_GETTER(wild_mouse)
TRACK_PAINT_FN_GETTER(multi_dimension_rc)
TRACK_PAINT_FN_GETTER(flying_rc)
TRACK_PAINT_FN_GETTER(virginia_reel)
TRACK_PAINT_FN_GETTER(splash_boats)
TRACK_PAINT_FN_GETTER(mini_helicopters)
TRACK_PAINT_FN_GETTER(lay_down_rc)
TRACK_PAINT_FN_GETTER(suspended_monorail)
TRACK_PAINT_FN_GETTER(reverser_rc)
TRACK_PAINT_FN_GETTER(heartline_twister_rc)
TRACK_PAINT_FN_GETTER(mini_golf)
TRACK_PAINT_FN_GETTER(giga_rc)
TRACK_PAINT_FN_GETTER(roto_drop)
TRACK_PAINT_FN_GETTER(flying_saucers)
TRACK_PAINT_FN_GETTER(crooked_house)
TRACK_PAINT_FN_GETTER(monorail_cycles)
TRACK_PAINT_FN_GETTER(compact_inverted_rc)
TRACK_PAINT_FN_GETTER(water_rc)
TRACK_PAINT_FN_GETTER(air_powered_vertical_rc)
TRACK_PAINT_FN_GETTER(inverted_hairpin_rc)
TRACK_PAINT_FN_GETTER(magic_carpet)
TRACK_PAINT_FN_GETTER(submarine_ride)
TRACK_PAINT_FN_GETTER(enterprise)
TRACK_PAINT_FN_GETTER(inverted_impulse_rc)
TRACK_PAINT_FN_GETTER(mini_rc)
TRACK_PAINT_FN_GETTER(mine_ride)
TRACK_PAINT_FN_GETTER(lim_launched_rc)
namespace HybridRC
{
    TRACK_PAINT_FUNCTION GetTrackPaintFunction(int32_t trackType)
    {
        return dummy_paint_function;
    }
} // namespace HybridRC
namespace SingleRailRC
{
    TRACK_PAINT_FUNCTION GetTrackPaintFunction(int32_t trackType)
    {
        return dummy_paint_function;
    }
} // namespace SingleRailRC

void Peep::SetState(PeepState new_state)
{
    // Probably overkill
    assert(false);
}
void Peep::InsertNewThought(PeepThoughtType thoughtType, uint8_t thoughtArguments)
{
    // Probably overkill
    assert(false);
}
int32_t network_get_player_index(uint32_t id)
{
    // Probably overkill
    assert(false);
    return -1;
}
const char* network_get_player_name(uint32_t index)
{
    // Probably overkill
    assert(false);
    return "vehicle test";
}
void crash_splash_create(const CoordsXYZ& splashPos)
{
    // Probably overkill
    assert(false);
}

void MoneyEffect::Update()
{
}
void VehicleCrashParticle::Update()
{
}
void CrashSplashParticle::Update()
{
}
void JumpingFountain::Update()
{
}
void Balloon::Update()
{
}
void Duck::Update()
{
}

WallElement* map_get_wall_element_at(const CoordsXYZD& wallCoords)
{
    // Safe to return nullptr here
    return nullptr;
}

int32_t wall_entry_get_door_sound(const rct_scenery_entry* wallEntry)
{
    // Probably overkill
    assert(false);
    return 0;
}

rct_scenery_entry* WallElement::GetEntry() const
{
    // Probably overkill
    assert(false);
    return nullptr;
}

uint8_t WallElement::GetAnimationFrame() const
{
    return 0;
}
void WallElement::SetAnimationIsBackwards(bool isBackwards)
{
    // Should not get called when stubbed wall_entry_get_door_sound returns nullptr
    assert(false);
}
void WallElement::SetAnimationFrame(uint8_t frameNum)
{
    // Should not get called when stubbed wall_entry_get_door_sound returns nullptr
    assert(false);
}
bool PathElement::IsBlockedByVehicle() const
{
    return (Flags2 & FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE) != 0;
}

void PathElement::SetIsBlockedByVehicle(bool isBlocked)
{
    if (isBlocked)
    {
        Flags2 |= FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE;
    }
    else
    {
        Flags2 &= ~FOOTPATH_ELEMENT_FLAGS2_BLOCKED_BY_VEHICLE;
    }
}

void context_broadcast_intent(Intent* intent)
{
    // Safe to ignore
}

void Ride::FormatNameTo(Formatter& ft) const
{
    // Probably overkill
    assert(false);
}

News::Item* News::AddItemToQueue(News::ItemType type, rct_string_id string_id, uint32_t assoc, const Formatter& formatter)
{
    // Probably overkill
    assert(false);
    return nullptr;
}

void Ride::Crash(uint8_t vehicleIndex)
{
    // TODO(janisozaur): needs validation
    assert(false);
}

void crashed_vehicle_particle_create(rct_vehicle_colour colours, const CoordsXYZ& vehiclePos)
{
}

bool ride_has_station_shelter(Ride* ride)
{
    // TODO(janisozaur): do something here
    return false;
}

int32_t get_booster_speed(uint8_t rideType, int32_t rawSpeed)
{
    // TODO(janisozaur): do something here
    return 0;
}

TileElement* get_station_platform(const CoordsXYRangedZ& coords)
{
    // TODO(janisozaur): do something here
    return {};
}

void decrement_guests_in_park()
{
    // Probably overkill
    assert(false);
}

void peep_sprite_remove(Peep* peep)
{
    // Probably overkill
    assert(false);
}

void Ride::FormatStatusTo(Formatter& ft) const
{
    // Probably overkill
    assert(false);
}

void viewports_invalidate(int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t maxZoom)
{
    // Safe to ignore
}

bool Duck::IsFlying()
{
    // Probably overkill
    assert(false);
    return false;
}

bool Peep::SetName(std::string_view value)
{
    // Probably overkill
    assert(false);
    return true;
}

template<> bool SpriteBase::Is<Peep>() const
{
    // Probably overkill
    assert(false);
    return sprite_identifier == SpriteIdentifier::Peep;
}

template<> bool SpriteBase::Is<Guest>() const
{
    // Probably overkill
    assert(false);
    auto peep = As<Peep>();
    return peep && peep->AssignedPeepType == PeepType::Guest;
}

template<> bool SpriteBase::Is<VehicleCrashParticle>() const
{
    // Probably overkill
    assert(false);
    auto* misc = As<MiscEntity>();
    return misc && misc->SubType == MiscEntityType::CrashedVehicleParticle;
}

template<> bool SpriteBase::Is<CrashSplashParticle>() const
{
    // Probably overkill
    assert(false);
    auto* misc = As<MiscEntity>();
    return misc && misc->SubType == MiscEntityType::CrashSplash;
}

template<> bool SpriteBase::Is<JumpingFountain>() const
{
    // Probably overkill
    assert(false);
    auto* misc = As<MiscEntity>();
    return misc
        && (misc->SubType == MiscEntityType::JumpingFountainSnow || misc->SubType == MiscEntityType::JumpingFountainWater);
}

template<> bool SpriteBase::Is<Balloon>() const
{
    // Probably overkill
    assert(false);
    auto* misc = As<MiscEntity>();
    return misc && misc->SubType == MiscEntityType::Balloon;
}

template<> bool SpriteBase::Is<Duck>() const
{
    // Probably overkill
    assert(false);
    auto* misc = As<MiscEntity>();
    return misc && misc->SubType == MiscEntityType::Duck;
}

template<> bool SpriteBase::Is<MoneyEffect>() const
{
    // Probably overkill
    assert(false);
    auto* misc = As<MiscEntity>();
    return misc && misc->SubType == MiscEntityType::MoneyEffect;
}

Guest* Peep::AsGuest()
{
    // Probably overkill
    assert(false);
    return nullptr;
}

int32_t Guest::GetEasterEggNameId() const
{
    // TODO(janisozaur): do something here
    return -1;
}

TEST(Vehicle, Create)
{
    [[maybe_unused]] Vehicle v{};
}

TEST(Vehicle, SimpleUpdate)
{
    Vehicle v{};
    v.Update();
}

TEST(Vehicle, CableLiftUpdate)
{
    reset_sprite_list();
    Ride& r = _rideList[0];
    r = Ride{};
    r.id = 0;
    Vehicle* v = cable_lift_segment_create(r, 0, 0, 0, 0, 0, 0, true);
    v->ride_subtype = RIDE_ENTRY_INDEX_NULL;
    v->status = Vehicle::Status::MovingToEndOfStation;
    v->next_vehicle_on_train = SPRITE_INDEX_NULL;
    v->Update();
}

static constexpr const CoordsXY word_9A3AB4[4] = {
    { 0, 0 },
    { 0, -96 },
    { -96, -96 },
    { -96, 0 },
};

// clang-format off
static constexpr const CoordsXY word_9A2A60[] = {
    { 0, 16 },
    { 16, 31 },
    { 31, 16 },
    { 16, 0 },
    { 16, 16 },
    { 64, 64 },
    { 64, -32 },
    { -32, -32 },
    { -32, 64 },
};
// clang-format on

static Vehicle* vehicle_create_car(
    ride_id_t rideIndex, int32_t vehicleEntryIndex, int32_t carIndex, int32_t vehicleIndex, const CoordsXYZ& carPosition,
    int32_t* remainingDistance, TrackElement* trackElement)
{
    const auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return nullptr;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return nullptr;

    auto vehicleEntry = &rideEntry->vehicles[vehicleEntryIndex];
    auto vehicle = &create_sprite(SpriteIdentifier::Vehicle, carIndex == 0 ? EntityListId::TrainHead : EntityListId::Vehicle)
                        ->vehicle;
    if (vehicle == nullptr)
        return nullptr;

    vehicle->sprite_identifier = SpriteIdentifier::Vehicle;
    vehicle->ride = rideIndex;
    vehicle->ride_subtype = ride->subtype;

    vehicle->vehicle_type = vehicleEntryIndex;
    vehicle->SubType = carIndex == 0 ? Vehicle::Type::Head : Vehicle::Type::Tail;
    vehicle->var_44 = ror32(vehicleEntry->spacing, 10) & 0xFFFF;
    auto edx = vehicleEntry->spacing >> 1;
    *remainingDistance -= edx;
    vehicle->remaining_distance = *remainingDistance;
    if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_GO_KART))
    {
        *remainingDistance -= edx;
    }

    // loc_6DD9A5:
    vehicle->sprite_width = vehicleEntry->sprite_width;
    vehicle->sprite_height_negative = vehicleEntry->sprite_height_negative;
    vehicle->sprite_height_positive = vehicleEntry->sprite_height_positive;
    vehicle->mass = vehicleEntry->car_mass;
    vehicle->num_seats = vehicleEntry->num_seats;
    vehicle->speed = vehicleEntry->powered_max_speed;
    vehicle->powered_acceleration = vehicleEntry->powered_acceleration;
    vehicle->velocity = 0;
    vehicle->acceleration = 0;
    vehicle->SwingSprite = 0;
    vehicle->SwingPosition = 0;
    vehicle->SwingSpeed = 0;
    vehicle->restraints_position = 0;
    vehicle->spin_sprite = 0;
    vehicle->spin_speed = 0;
    vehicle->sound2_flags = 0;
    vehicle->sound1_id = OpenRCT2::Audio::SoundId::Null;
    vehicle->sound2_id = OpenRCT2::Audio::SoundId::Null;
    vehicle->next_vehicle_on_train = SPRITE_INDEX_NULL;
    vehicle->var_C4 = 0;
    vehicle->animation_frame = 0;
    vehicle->var_C8 = 0;
    vehicle->scream_sound_id = OpenRCT2::Audio::SoundId::Null;
    vehicle->vehicle_sprite_type = 0;
    vehicle->bank_rotation = 0;
    vehicle->target_seat_rotation = 4;
    vehicle->seat_rotation = 4;
    for (int32_t i = 0; i < 32; i++)
    {
        vehicle->peep[i] = SPRITE_INDEX_NULL;
    }

    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT)
    {
        // loc_6DDCA4:
        vehicle->TrackSubposition = VehicleTrackSubposition::Default;
        int32_t direction = trackElement->GetDirection();
        auto dodgemPos = carPosition + CoordsXYZ{ word_9A3AB4[direction], 0 };
        vehicle->TrackLocation = dodgemPos;
        vehicle->current_station = trackElement->GetStationIndex();

        dodgemPos.z += RideTypeDescriptors[ride->type].Heights.VehicleZOffset;

        vehicle->SetTrackDirection(0);
        vehicle->SetTrackType(trackElement->GetTrackType());
        vehicle->track_progress = 0;
        vehicle->SetState(Vehicle::Status::MovingToEndOfStation);
        vehicle->update_flags = 0;

        CoordsXY chosenLoc;
        // loc_6DDD26:
        do
        {
            vehicle->sprite_direction = scenario_rand() & 0x1E;
            chosenLoc.y = dodgemPos.y + (scenario_rand() & 0xFF);
            chosenLoc.x = dodgemPos.x + (scenario_rand() & 0xFF);
        } while (vehicle->DodgemsCarWouldCollideAt(chosenLoc, nullptr));

        vehicle->MoveTo({ chosenLoc, dodgemPos.z });
    }
    else
    {
        VehicleTrackSubposition subposition = VehicleTrackSubposition::Default;
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_CHAIRLIFT)
        {
            subposition = VehicleTrackSubposition::ChairliftGoingOut;
        }

        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_GO_KART)
        {
            // Choose which lane Go Kart should start in
            subposition = VehicleTrackSubposition::GoKartsLeftLane;
            if (vehicleIndex & 1)
            {
                subposition = VehicleTrackSubposition::GoKartsRightLane;
            }
        }
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_MINI_GOLF)
        {
            subposition = VehicleTrackSubposition::MiniGolfStart9;
            vehicle->var_D3 = 0;
            vehicle->mini_golf_current_animation = 0;
            vehicle->mini_golf_flags = 0;
        }
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_4)
        {
            if (vehicle->IsHead())
            {
                subposition = VehicleTrackSubposition::ReverserRCFrontBogie;
            }
        }
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_5)
        {
            subposition = VehicleTrackSubposition::ReverserRCRearBogie;
        }
        vehicle->TrackSubposition = subposition;

        auto chosenLoc = carPosition;
        vehicle->TrackLocation = chosenLoc;

        int32_t direction = trackElement->GetDirection();
        vehicle->sprite_direction = direction << 3;

        if (ride->type == RIDE_TYPE_SPACE_RINGS)
        {
            direction = 4;
        }
        else
        {
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL))
            {
                if (RideTypeDescriptors[ride->type].StartTrackPiece != FLAT_TRACK_ELEM_1_X_4_B)
                {
                    if (RideTypeDescriptors[ride->type].StartTrackPiece != FLAT_TRACK_ELEM_1_X_4_A)
                    {
                        if (ride->type == RIDE_TYPE_ENTERPRISE)
                        {
                            direction += 5;
                        }
                        else
                        {
                            direction = 4;
                        }
                    }
                }
            }
        }

        chosenLoc += CoordsXYZ{ word_9A2A60[direction], RideTypeDescriptors[ride->type].Heights.VehicleZOffset };

        vehicle->current_station = trackElement->GetStationIndex();

        vehicle->MoveTo(chosenLoc);
        vehicle->SetTrackType(trackElement->GetTrackType());
        vehicle->SetTrackDirection(vehicle->sprite_direction >> 3);
        vehicle->track_progress = 31;
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_MINI_GOLF)
        {
            vehicle->track_progress = 15;
        }
        vehicle->update_flags = VEHICLE_UPDATE_FLAG_COLLISION_DISABLED;
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET)
        {
            if (trackElement->IsInverted())
            {
                vehicle->SetUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES);
            }
        }
        vehicle->SetState(Vehicle::Status::MovingToEndOfStation);
    }

    // loc_6DDD5E:
    vehicle->num_peeps = 0;
    vehicle->next_free_seat = 0;
    vehicle->BoatLocation.setNull();
    return vehicle;
}

rct_ride_entry* Ride::GetRideEntry() const
{
    return get_ride_entry(subtype);
}

TEST(Vehicle, VehicleCreateSimple)
{
    reset_sprite_list();
    Ride& r = _rideList[0];
    r = Ride{};
    r.id = 0;
    r.type = RIDE_TYPE_MONORAIL;
    int32_t rd{};
    TrackElement te{};
    vehicle_create_car(0, 0, 0, 0, {}, &rd, &te);
}

static std::vector<std::pair<std::string, int>> _goto_callsites;

// Silence the -Werror=missing-declarations error
void hit_goto_callsite(std::string target, int callsite_id);
void hit_goto_callsite(std::string target, int callsite_id)
{
    _goto_callsites.push_back(std::make_pair(target, callsite_id));
}

TEST(Vehicle, GotoCallsite)
{
    _goto_callsites.clear();
    reset_sprite_list();
    Ride& r = _rideList[0];
    r = Ride{};
    r.id = 0;
    r.type = RIDE_TYPE_MONORAIL;
    r.mode = RideMode::Race;
    r.lifecycle_flags = RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
    int32_t rd{};
    TrackElement te{};
    Vehicle* v = vehicle_create_car(0, 0, 0, 0, {}, &rd, &te);
    v->status = Vehicle::Status::Arriving;
    v->Update();
    EXPECT_EQ(_goto_callsites.size(), 1);
    std::vector<std::pair<std::string, int>> expected_results;
    expected_results.push_back(std::make_pair("loc_6D8E36", 1));
    EXPECT_EQ(_goto_callsites, expected_results);
}
