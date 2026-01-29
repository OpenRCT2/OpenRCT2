/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"
#include "../core/FlagHolder.hpp"
#include "../entity/Yaw.hpp"
#include "../world/Location.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace OpenRCT2::Audio
{
    enum class SoundId : uint8_t;
}

enum class SoundRange : uint8_t;

enum class CarEntryAnimation : uint8_t
{
    None = 0,
    SimpleVehicle,
    SteamLocomotive,
    SwanBoat,
    MonorailCycle,
    MultiDimension,
    ObservationTower,
    AnimalFlying,
    Count,
};

enum class CarEntryFlag : uint8_t
{
    /*
     * Set on powered vehicles that freely accelerate downhill, like the ghost train and log flume. When unset, powered
     * vehicles apply brakes when going downhill.
     */
    isPoweredRideWithUnrestrictedGravity,
    hasNoUpstopWheels,
    hasNoUpstopWheelsBobsleigh,
    isMiniGolf,
    isReverserCoasterBogie,
    isReverserCoasterPassengerCar,
    /*
     * Set on vehicles that support running inverted for extended periods of time,i.e. the Flying,
     * Lay-down and Multi-dimension RCs
     */
    hasInvertedSpriteSet,
    // When set the vehicle has an additional frame for when in use. Used only by dodgems.
    hasDodgemInUseLights,
    // Not used anymore - every vehicle works with doors in OpenRCT2
    allowDoorsDeprecated,
    enableTertiaryColour,
    // Only used during loading of the objects
    recalculateSpriteBounds,
    // Instead of the default 32 rotation frames. Only used for boat hire and works only for non sloped sprites.
    use16RotationFrames,
    /*
     * Setting this will cause the game to set carEntry->num_vertical_frames to carEntry->num_vertical_frames_override,
     * rather than determining it itself
     */
    overrideNumberOfVerticalFrames,
    /*
     * Used together with hasInvertedSpriteSet and recalculateSpriteBounds, includes the inverted sprites into the function
     * that recalculates the sprite bounds if the vehicle combines the spinning carriage and non-spinning undercarriage
     * in the same sprite.
     */
    spriteBoundsIncludeInvertedSet,
    hasSpinningCombinedWithNonSpinning,
    isLift,
    enableTrimColour,
    hasSwinging,
    hasSpinning,
    isPowered,
    // Only valid for front/default car of train
    hasScreamingRiders,
    // Suspended swinging coaster, or bobsleigh if useSlideSwing flag is also enabled
    useSuspendedSwing,
    useBoatHireCollisionDetection,
    // Set on animated vehicles like the Multi-dimension coaster trains, Miniature Railway locomotives and Helicycles
    hasVehicleAnimation,
    // Set when the animation updates rider sprite positions
    hasRiderAnimation,
    useWoodenWildMouseSwing,
    // Peep loading positions have x and y coordinates. Normal rides just have offsets.
    loadingWaypoints,
    /*
     * Set on dinghy slides. They have their own swing value calculations and have a different amount of images.
     * Also set on bobsleighs together with the suspendedSwing flag.
     */
    useSlideSwing,
    isChairlift,
    // Set on rides where water would provide continuous propulsion
    isWaterRide,
    isGoKart,
    useDodgemCarPlacement,
    enableBodyColour,
};
using CarEntryFlags = FlagHolder<uint64_t, CarEntryFlag>;

/*
 * When adding a sprite group, add multiplier to SpriteGroupMultiplier in RideObject.cpp and add sprite group data to cable
 * lift hill vehicle in RideData.cpp and update the SpriteGroups interface in distribution/openrct2.d.ts
 */
enum class SpriteGroupType : uint8_t
{
    SlopeFlat = 0,
    Slopes12,
    Slopes25,
    Slopes42,
    Slopes60,
    Slopes75,
    Slopes90,
    SlopesLoop,
    SlopeInverted,
    Slopes8,
    Slopes16,
    Slopes50,
    FlatBanked22,
    FlatBanked45,
    FlatBanked67,
    FlatBanked90,
    InlineTwists,
    Slopes12Banked22,
    Slopes8Banked22,
    Slopes25Banked22,
    Slopes8Banked45,
    Slopes16Banked22,
    Slopes16Banked45,
    Slopes25Banked45,
    Slopes12Banked45,
    Slopes25Banked67,
    Slopes25Banked90,
    Slopes25InlineTwists,
    Slopes42Banked22,
    Slopes42Banked45,
    Slopes42Banked67,
    Slopes42Banked90,
    Slopes60Banked22,
    Slopes50Banked45,
    Slopes50Banked67,
    Slopes50Banked90,
    Corkscrews,
    RestraintAnimation,
    CurvedLiftHillUp,
    CurvedLiftHillDown,
    Count
};

static constexpr const char* SpriteGroupNames[] = { "slopeFlat",        "slopes12",
                                                    "slopes25",         "slopes42",
                                                    "slopes60",         "slopes75",
                                                    "slopes90",         "slopesLoop",
                                                    "slopeInverted",    "slopes8",
                                                    "slopes16",         "slopes50",
                                                    "flatBanked22",     "flatBanked45",
                                                    "flatBanked67",     "flatBanked90",
                                                    "inlineTwists",     "slopes12Banked22",
                                                    "slopes8Banked22",  "slopes25Banked22",
                                                    "slopes8Banked45",  "slopes16Banked22",
                                                    "slopes16Banked45", "slopes25Banked45",
                                                    "slopes12Banked45", "slopes25Banked67",
                                                    "slopes25Banked90", "slopes25InlineTwists",
                                                    "slopes42Banked22", "slopes42Banked45",
                                                    "slopes42Banked67", "slopes42Banked90",
                                                    "slopes60Banked22", "slopes50Banked45",
                                                    "slopes50Banked67", "slopes50Banked90",
                                                    "corkscrews",       "restraintAnimation",
                                                    "curvedLiftHillUp", "curvedLiftHillDown" };
static_assert(std::size(SpriteGroupNames) == EnumValue(SpriteGroupType::Count));

struct VehicleSpriteGroup
{
    uint32_t imageId{};
    OpenRCT2::Entity::Yaw::SpritePrecision spritePrecision{};
    bool Enabled() const
    {
        return spritePrecision != OpenRCT2::Entity::Yaw::SpritePrecision::None;
    }
};

/**
 * Ride type vehicle structure.
 */
struct CarEntry
{
    uint16_t TabRotationMask;
    uint32_t spacing;
    uint16_t car_mass;
    int8_t tab_height;
    uint8_t num_seats;
    uint8_t spriteWidth;
    uint8_t spriteHeightNegative;
    uint8_t spriteHeightPositive;
    CarEntryAnimation animation;
    CarEntryFlags flags;
    uint16_t base_num_frames; // The number of sprites of animation or swinging per rotation frame
    uint32_t base_image_id;
    VehicleSpriteGroup SpriteGroups[EnumValue(SpriteGroupType::Count)];
    uint32_t NumCarImages;
    uint8_t no_seating_rows;
    uint8_t spinning_inertia;
    uint8_t spinning_friction;
    OpenRCT2::Audio::SoundId friction_sound_id; // Only valid for front/default car of train
    uint8_t ReversedCarIndex; // When the car is reversed (using a turntable or reverser), it will be changed to this car.
    SoundRange soundRange;
    uint8_t double_sound_frequency; // (Doubles the velocity when working out the sound frequency {used on go karts})
    uint8_t powered_acceleration;
    uint8_t powered_max_speed;
    uint8_t PaintStyle;
    uint8_t effect_visual;
    uint8_t draw_order;
    uint8_t num_vertical_frames_override; // A custom number that can be used rather than letting RCT2 determine it.
    // Needs the overrideNumberOfVerticalFrames CarEntryFlag to be set.
    uint8_t peep_loading_waypoint_segments;
    uint16_t AnimationSpeed;
    uint8_t AnimationFrames;
    struct
    {
        int8_t Longitudinal;
        int8_t Vertical;
    } SteamEffect;
    uint8_t spinningNumFrames;
    std::vector<std::array<CoordsXY, 3>> peep_loading_waypoints = {};
    std::vector<int8_t> peep_loading_positions = {};

    uint32_t NumRotationSprites(SpriteGroupType rotationType) const;
    int32_t SpriteByYaw(int32_t yaw, SpriteGroupType rotationType) const;
    bool GroupEnabled(SpriteGroupType rotationType) const;
    uint32_t GroupImageId(SpriteGroupType spriteGroup) const;
    uint32_t SpriteOffset(SpriteGroupType spriteGroup, int32_t imageDirection, uint8_t rankIndex) const;

    bool isVisible() const;
};

void CarEntrySetImageMaxSizes(CarEntry& carEntry, int32_t numImages);
