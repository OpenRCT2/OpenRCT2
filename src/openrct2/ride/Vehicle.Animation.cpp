/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Vehicle.h"

#include "../GameState.h"
#include "../audio/Audio.h"
#include "../entity/Particle.h"
#include "../entity/Yaw.hpp"
#include "../math/Trigonometry.hpp"
#include "../world/Map.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "Ride.h"
#include "RideData.h"
#include "RideEntry.h"
#include "Track.h"
#include "TrackData.h"
#include "VehicleData.h"

#include <cassert>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::TrackMetaData;

constexpr int16_t kVehicleMaxSpinSpeed = 1536;
constexpr int16_t kVehicleMinSpinSpeed = -kVehicleMaxSpinSpeed;

/**
 *
 *  rct2: 0x006D9249
 */
void Vehicle::UpdateSwinging()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    auto rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
        return;

    // SubState for this ride means swinging state
    // 0 == first swing
    // 3 == full swing
    uint8_t swingState = sub_state;
    if (rideEntry->flags.has(RideEntryFlag::inverterShipSwingMode))
    {
        swingState += 4;
        if (rideEntry->flags.has(RideEntryFlag::magicCarpetSwingMode))
            swingState += 4;
    }

    const auto spriteMap = kSwingingTimeToSpriteMaps[swingState];
    int8_t spriteType = spriteMap[current_time + 1];

    // 0x80 indicates that a complete swing has been
    // completed and the next swing can start
    if (spriteType != -128)
    {
        current_time++;
        if (static_cast<uint8_t>(spriteType) != flatRideAnimationFrame)
        {
            // Used to know which sprite to draw
            flatRideAnimationFrame = static_cast<uint8_t>(spriteType);
            Invalidate();
        }
        return;
    }

    current_time = -1;
    NumSwings++;
    if (curRide->status != RideStatus::closed)
    {
        // It takes 3 swings to get into full swing
        // ride->rotations already takes this into account
        if (NumSwings + 3 < curRide->rotations)
        {
            // Go to the next swing state until we
            // are at full swing.
            if (sub_state != 3)
            {
                sub_state++;
            }
            UpdateSwinging();
            return;
        }
    }

    // To get to this part of the code the
    // swing has to be in slowing down phase
    if (sub_state == 0)
    {
        SetState(Status::arriving);
        var_C0 = 0;
        return;
    }
    // Go towards first swing state
    sub_state--;
    UpdateSwinging();
}

/**
 *
 *  rct2: 0x006D9413
 */
void Vehicle::UpdateFerrisWheelRotating()
{
    if (_vehicleBreakdown == 0)
        return;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if ((ferris_wheel_var_1 -= 1) != 0)
        return;

    int8_t curFerrisWheelVar0 = ferris_wheel_var_0;

    if (curFerrisWheelVar0 == 3)
    {
        ferris_wheel_var_0 = curFerrisWheelVar0;
        ferris_wheel_var_1 = curFerrisWheelVar0;
    }
    else if (curFerrisWheelVar0 < 3)
    {
        if (curFerrisWheelVar0 != -8)
            curFerrisWheelVar0--;
        ferris_wheel_var_0 = curFerrisWheelVar0;
        ferris_wheel_var_1 = -curFerrisWheelVar0;
    }
    else
    {
        curFerrisWheelVar0--;
        ferris_wheel_var_0 = curFerrisWheelVar0;
        ferris_wheel_var_1 = curFerrisWheelVar0;
    }

    uint8_t rotation = flatRideAnimationFrame;
    if (curRide->mode == RideMode::forwardRotation)
        rotation++;
    else
        rotation--;

    rotation &= 0x7F;
    flatRideAnimationFrame = rotation;

    if (rotation == sub_state)
        NumRotations++;

    Invalidate();

    uint8_t subState = sub_state;
    if (curRide->mode == RideMode::forwardRotation)
        subState++;
    else
        subState--;
    subState &= 0x7F;

    if (subState == flatRideAnimationFrame)
    {
        bool shouldStop = true;
        if (curRide->status != RideStatus::closed)
        {
            if (NumRotations < curRide->rotations)
                shouldStop = false;
        }

        if (shouldStop)
        {
            curFerrisWheelVar0 = ferris_wheel_var_0;
            ferris_wheel_var_0 = -abs(curFerrisWheelVar0);
            ferris_wheel_var_1 = abs(curFerrisWheelVar0);
        }
    }

    if (ferris_wheel_var_0 != -8)
        return;

    subState = sub_state;
    if (curRide->mode == RideMode::forwardRotation)
        subState += 8;
    else
        subState -= 8;
    subState &= 0x7F;

    if (subState != flatRideAnimationFrame)
        return;

    SetState(Status::arriving);
    var_C0 = 0;
}

/**
 *
 *  rct2: 0x006D94F2
 */
void Vehicle::UpdateSimulatorOperating()
{
    if (_vehicleBreakdown == 0)
        return;

    assert(current_time >= -1);
    assert(current_time < MotionSimulatorTimeToSpriteMapCount);
    uint8_t al = MotionSimulatorTimeToSpriteMap[current_time + 1];
    if (al != 0xFF)
    {
        current_time++;
        if (al == flatRideAnimationFrame)
            return;
        flatRideAnimationFrame = al;
        Invalidate();
        return;
    }

    SetState(Status::arriving);
    var_C0 = 0;
}

void UpdateRotatingDefault(Vehicle& vehicle)
{
    vehicle.sub_state = 1;
    vehicle.UpdateRotating();
}

void UpdateRotatingEnterprise(Vehicle& vehicle)
{
    if (vehicle.sub_state == 2)
    {
        vehicle.SetState(Vehicle::Status::arriving);
        vehicle.var_C0 = 0;
        return;
    }

    UpdateRotatingDefault(vehicle);
}

/**
 *
 *  rct2: 0x006D92FF
 */
void Vehicle::UpdateRotating()
{
    if (_vehicleBreakdown == 0)
        return;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    auto rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }

    const uint8_t* timeToSpriteMap;
    if (rideEntry->flags.has(RideEntryFlag::hasTwistRotationType))
    {
        timeToSpriteMap = kTwistTimeToSpriteMaps[sub_state];
    }
    else if (rideEntry->flags.has(RideEntryFlag::hasEnterpriseRotationType))
    {
        timeToSpriteMap = kEnterpriseTimeToSpriteMaps[sub_state];
    }
    else
    {
        timeToSpriteMap = kMerryGoRoundTimeToSpriteMaps[sub_state];
    }

    uint16_t time = current_time;
    if (_vehicleBreakdown == BREAKDOWN_CONTROL_FAILURE)
    {
        time += (curRide->breakdownSoundModifier >> 6) + 1;
    }
    time++;

    uint8_t sprite = timeToSpriteMap[time];
    if (sprite != 0xFF)
    {
        current_time = time;
        if (sprite == flatRideAnimationFrame)
            return;
        flatRideAnimationFrame = sprite;
        Invalidate();
        return;
    }

    current_time = -1;
    NumRotations++;
    if (_vehicleBreakdown != BREAKDOWN_CONTROL_FAILURE)
    {
        bool shouldStop = true;
        if (curRide->status != RideStatus::closed)
        {
            sprite = NumRotations + 1;
            if (curRide->getRideTypeDescriptor().specialType == RtdSpecialType::enterprise)
                sprite += 9;

            if (sprite < curRide->rotations)
                shouldStop = false;
        }

        if (shouldStop)
        {
            if (sub_state == 2)
            {
                SetState(Status::arriving);
                var_C0 = 0;
                return;
            }
            sub_state++;
            UpdateRotating();
            return;
        }
    }

    const auto& rtd = GetRideTypeDescriptor(curRide->type);
    rtd.UpdateRotating(*this);
}

/**
 *
 *  rct2: 0x006D97CB
 */
void Vehicle::UpdateSpaceRingsOperating()
{
    if (_vehicleBreakdown == 0)
        return;

    uint8_t spriteType = kSpaceRingsTimeToSpriteMap[current_time + 1];
    if (spriteType != 255)
    {
        current_time++;
        if (spriteType != flatRideAnimationFrame)
        {
            flatRideAnimationFrame = spriteType;
            Invalidate();
        }
    }
    else
    {
        SetState(Status::arriving);
        var_C0 = 0;
    }
}

/**
 *
 *  rct2: 0x006D9641
 */
void Vehicle::UpdateHauntedHouseOperating()
{
    if (_vehicleBreakdown == 0)
        return;

    if (flatRideAnimationFrame != 0)
    {
        if (getGameState().currentTicks & 1)
        {
            flatRideAnimationFrame++;
            Invalidate();

            if (flatRideAnimationFrame == 19)
                flatRideAnimationFrame = 0;
        }
    }

    if (current_time + 1 > 1500)
    {
        SetState(Status::arriving);
        var_C0 = 0;
        return;
    }

    current_time++;
    switch (current_time)
    {
        case 45:
            Play3D(SoundId::hauntedHouseScare, GetLocation());
            break;
        case 75:
            flatRideAnimationFrame = 1;
            Invalidate();
            break;
        case 400:
            Play3D(SoundId::hauntedHouseScream1, GetLocation());
            break;
        case 745:
            Play3D(SoundId::hauntedHouseScare, GetLocation());
            break;
        case 775:
            flatRideAnimationFrame = 1;
            Invalidate();
            break;
        case 1100:
            Play3D(SoundId::hauntedHouseScream2, GetLocation());
            break;
    }
}

/**
 *
 *  rct2: 0x006d9781
 */
void Vehicle::UpdateCrookedHouseOperating()
{
    if (_vehicleBreakdown == 0)
        return;

    // Originally used an array of size 1 at 0x009A0AC4 and passed the sub state into it.
    if (static_cast<uint16_t>(current_time + 1) > 600)
    {
        SetState(Status::arriving);
        var_C0 = 0;
        return;
    }

    current_time++;
}

/**
 *
 *  rct2: 0x006D9547
 */
void Vehicle::UpdateTopSpinOperating()
{
    if (_vehicleBreakdown == 0)
        return;

    const TopSpinTimeToSpriteMap* sprite_map = kTopSpinTimeToSpriteMaps[sub_state];
    uint8_t rotation = sprite_map[current_time + 1].arm_rotation;
    if (rotation != 0xFF)
    {
        current_time = current_time + 1;
        if (rotation != flatRideAnimationFrame)
        {
            flatRideAnimationFrame = rotation;
            Invalidate();
        }
        rotation = sprite_map[current_time].bank_rotation;
        if (rotation != flatRideSecondaryAnimationFrame)
        {
            flatRideSecondaryAnimationFrame = rotation;
            Invalidate();
        }
        return;
    }

    SetState(Status::arriving);
    var_C0 = 0;
}

/**
 *
 *  rct2: 0x006D95AD
 */
void Vehicle::UpdateShowingFilm()
{
    if (_vehicleBreakdown == 0)
        return;

    int32_t totalTime = kRideFilmLength[sub_state];
    int32_t currentTime = current_time + 1;
    if (currentTime <= totalTime)
    {
        current_time = currentTime;
    }
    else
    {
        SetState(Status::arriving);
        var_C0 = 0;
    }
}

/**
 *
 *  rct2: 0x006D95F7
 */
void Vehicle::UpdateDoingCircusShow()
{
    if (_vehicleBreakdown == 0)
        return;

    int32_t currentTime = current_time + 1;
    if (currentTime <= 5000)
    {
        current_time = currentTime;
    }
    else
    {
        SetState(Status::arriving);
        var_C0 = 0;
    }
}

int32_t Vehicle::GetSwingAmount() const
{
    switch (GetTrackType())
    {
        case TrackElemType::leftQuarterTurn5Tiles:
        case TrackElemType::bankedLeftQuarterTurn5Tiles:
        case TrackElemType::leftQuarterTurn5TilesUp25:
        case TrackElemType::leftQuarterTurn5TilesDown25:
        case TrackElemType::leftQuarterTurn5TilesCovered:
        case TrackElemType::leftHalfBankedHelixUpLarge:
        case TrackElemType::leftHalfBankedHelixDownLarge:
        case TrackElemType::leftQuarterBankedHelixLargeUp:
        case TrackElemType::leftQuarterBankedHelixLargeDown:
        case TrackElemType::leftQuarterHelixLargeUp:
        case TrackElemType::leftQuarterHelixLargeDown:
        case TrackElemType::leftBankedQuarterTurn5TileUp25:
        case TrackElemType::leftBankedQuarterTurn5TileDown25:
            // Loc6D67E1
            return 14;

        case TrackElemType::rightQuarterTurn5Tiles:
        case TrackElemType::bankedRightQuarterTurn5Tiles:
        case TrackElemType::rightQuarterTurn5TilesUp25:
        case TrackElemType::rightQuarterTurn5TilesDown25:
        case TrackElemType::rightQuarterTurn5TilesCovered:
        case TrackElemType::rightHalfBankedHelixUpLarge:
        case TrackElemType::rightHalfBankedHelixDownLarge:
        case TrackElemType::rightQuarterBankedHelixLargeUp:
        case TrackElemType::rightQuarterBankedHelixLargeDown:
        case TrackElemType::rightQuarterHelixLargeUp:
        case TrackElemType::rightQuarterHelixLargeDown:
        case TrackElemType::rightBankedQuarterTurn5TileUp25:
        case TrackElemType::rightBankedQuarterTurn5TileDown25:
            // Loc6D6804
            return -14;

        case TrackElemType::sBendLeft:
        case TrackElemType::sBendLeftCovered:
            // Loc6D67EF
            if (track_progress < 48)
            {
                return 14;
            }
            return -15;

        case TrackElemType::sBendRight:
        case TrackElemType::sBendRightCovered:
            // Loc6D67CC
            if (track_progress < 48)
            {
                return -14;
            }
            return 15;

        case TrackElemType::leftQuarterTurn3Tiles:
        case TrackElemType::leftBankedQuarterTurn3Tiles:
        case TrackElemType::leftQuarterTurn3TilesUp25:
        case TrackElemType::leftQuarterTurn3TilesDown25:
        case TrackElemType::leftQuarterTurn3TilesCovered:
        case TrackElemType::leftHalfBankedHelixUpSmall:
        case TrackElemType::leftHalfBankedHelixDownSmall:
        case TrackElemType::leftBankToLeftQuarterTurn3TilesUp25:
        case TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank:
        case TrackElemType::leftCurvedLiftHill:
        case TrackElemType::leftBankedQuarterTurn3TileUp25:
        case TrackElemType::leftBankedQuarterTurn3TileDown25:
            // Loc6D67BE
            return 13;

        case TrackElemType::rightQuarterTurn3Tiles:
        case TrackElemType::rightBankedQuarterTurn3Tiles:
        case TrackElemType::rightQuarterTurn3TilesUp25:
        case TrackElemType::rightQuarterTurn3TilesDown25:
        case TrackElemType::rightQuarterTurn3TilesCovered:
        case TrackElemType::rightHalfBankedHelixUpSmall:
        case TrackElemType::rightHalfBankedHelixDownSmall:
        case TrackElemType::rightBankToRightQuarterTurn3TilesUp25:
        case TrackElemType::rightQuarterTurn3TilesDown25ToRightBank:
        case TrackElemType::rightCurvedLiftHill:
        case TrackElemType::rightBankedQuarterTurn3TileUp25:
        case TrackElemType::rightBankedQuarterTurn3TileDown25:
            // Loc6D67B0
            return -13;

        case TrackElemType::leftQuarterTurn1Tile:
        case TrackElemType::leftQuarterTurn1TileUp60:
        case TrackElemType::leftQuarterTurn1TileDown60:
            // Loc6D67A2
            return 12;

        case TrackElemType::rightQuarterTurn1Tile:
        case TrackElemType::rightQuarterTurn1TileUp60:
        case TrackElemType::rightQuarterTurn1TileDown60:
            // Loc6D6794
            return -12;

        case TrackElemType::leftEighthToDiag:
        case TrackElemType::leftEighthToOrthogonal:
        case TrackElemType::leftEighthBankToDiag:
        case TrackElemType::leftEighthBankToOrthogonal:
            // Loc6D67D3
            return 15;

        case TrackElemType::rightEighthToDiag:
        case TrackElemType::rightEighthToOrthogonal:
        case TrackElemType::rightEighthBankToDiag:
        case TrackElemType::rightEighthBankToOrthogonal:
            // Loc6D67F6
            return -15;
        default:
            return 0;
    }
}

static uint8_t GetSwingSprite(int16_t swingPosition)
{
    if (swingPosition < -10010)
        return 11;
    if (swingPosition > 10010)
        return 12;

    if (swingPosition < -8190)
        return 9;
    if (swingPosition > 8190)
        return 10;

    if (swingPosition < -6370)
        return 7;
    if (swingPosition > 6370)
        return 8;

    if (swingPosition < -4550)
        return 5;
    if (swingPosition > 4550)
        return 6;

    if (swingPosition < -2730)
        return 3;
    if (swingPosition > 2730)
        return 4;

    if (swingPosition < -910)
        return 1;
    if (swingPosition > 910)
        return 2;

    return 0;
}

/**
 *
 *  rct2: 0x006D6776
 */
void Vehicle::UpdateSwingingCar()
{
    int32_t dword_F64E08 = abs(_vehicleVelocityF64E08);
    if (flags.has(VehicleFlag::carIsReversed))
    {
        dword_F64E08 *= -1;
    }
    SwingSpeed += (-SwingPosition) >> 6;
    int32_t swingAmount = GetSwingAmount();
    if (swingAmount < 0)
    {
        SwingSpeed -= dword_F64E08 >> (-swingAmount);
    }
    else if (swingAmount > 0)
    {
        SwingSpeed += dword_F64E08 >> swingAmount;
    }

    auto carEntry = Entry();
    if (carEntry == nullptr)
    {
        return;
    }
    int16_t dx = 3185;
    if (carEntry->flags.has(CarEntryFlag::useSuspendedSwing))
    {
        dx = 5006;
    }
    if (carEntry->flags.has(CarEntryFlag::useWoodenWildMouseSwing))
    {
        dx = 1820;
    }
    int16_t cx = -dx;

    if (carEntry->flags.has(CarEntryFlag::useSlideSwing))
    {
        dx = 5370;
        cx = -5370;

        auto trackType = GetTrackType();
        switch (trackType)
        {
            case TrackElemType::bankedLeftQuarterTurn5Tiles:
            case TrackElemType::leftBank:
            case TrackElemType::leftBankedQuarterTurn3Tiles:
                dx = 10831;
                cx = -819;
                break;
            case TrackElemType::bankedRightQuarterTurn5Tiles:
            case TrackElemType::rightBank:
            case TrackElemType::rightBankedQuarterTurn3Tiles:
                dx = 819;
                cx = -10831;
                break;
            default:
                break;
        }

        if (TrackTypeIsStation(trackType) || TrackTypeIsBrakes(trackType) || TrackTypeIsBlockBrakes(trackType))
        {
            dx = 0;
            cx = 0;
        }

        if (flags.has(VehicleFlag::onLiftHill))
        {
            dx = 0;
            cx = 0;
        }
    }

    SwingPosition += SwingSpeed;
    SwingSpeed -= SwingSpeed >> 5;

    if (SwingPosition > dx)
    {
        SwingPosition = dx;
        SwingSpeed = 0;
    }
    if (SwingPosition < cx)
    {
        SwingPosition = cx;
        SwingSpeed = 0;
    }

    uint8_t swingSprite = GetSwingSprite(SwingPosition);

    if (swingSprite != SwingSprite)
    {
        SwingSprite = swingSprite;
        Invalidate();
    }
}

/**
 *
 *  rct2: 0x006D661F
 */
void Vehicle::UpdateSpinningCar()
{
    if (flags.has(VehicleFlag::spinningIsLocked))
    {
        spin_speed = 0;
        return;
    }

    auto carEntry = Entry();
    if (carEntry == nullptr)
    {
        return;
    }
    int32_t spinningInertia = carEntry->spinning_inertia;
    auto trackType = GetTrackType();
    int32_t dword_F64E08 = _vehicleVelocityF64E08;
    int32_t spinSpeed{};
    // An L spin adds to the spin speed, R does the opposite
    // The number indicates how much right shift of the velocity will become spin
    // The bigger the number the less change in spin.

    const auto& ted = GetTrackElementDescriptor(trackType);
    switch (ted.spinFunction)
    {
        case SpinFunction::RC:
            // On a rotation control track element
            spinningInertia += 6;
            spinSpeed = dword_F64E08 >> spinningInertia;
            // Alternate the spin direction (roughly). Perhaps in future save a value to the track
            if (Id.ToUnderlying() & 1)
            {
                spin_speed -= spinSpeed;
            }
            else
            {
                spin_speed += spinSpeed;
            }
            break;
        case SpinFunction::R5:
            // It looks like in the original there was going to be special code for whirlpool
            // this has been removed and just uses R5
            spinningInertia += 5;
            spin_speed -= dword_F64E08 >> spinningInertia;
            break;
        case SpinFunction::L5:
            spinningInertia += 5;
            spin_speed += dword_F64E08 >> spinningInertia;
            break;
        case SpinFunction::R7:
            spinningInertia += 7;
            spin_speed -= dword_F64E08 >> spinningInertia;
            break;
        case SpinFunction::L7:
            spinningInertia += 7;
            spin_speed += dword_F64E08 >> spinningInertia;
            break;
        case SpinFunction::RL:
            // Right Left Curve Track Piece
            if (track_progress < 48)
            {
                // R8
                spinningInertia += 8;
                spin_speed -= dword_F64E08 >> spinningInertia;
                break;
            }
            [[fallthrough]];
        case SpinFunction::L9:
            spinningInertia += 9;
            spin_speed += dword_F64E08 >> spinningInertia;
            break;
        case SpinFunction::L8:
            spinningInertia += 8;
            spin_speed += dword_F64E08 >> spinningInertia;
            break;
        case SpinFunction::SP:
            // On rapids spin after fully on them
            if (track_progress > 22)
            {
                // L5
                spinningInertia += 5;
                spin_speed += dword_F64E08 >> spinningInertia;
            }
            break;
        case SpinFunction::LR:
            // Left Right Curve Track Piece
            if (track_progress < 48)
            {
                // L8
                spinningInertia += 8;
                spin_speed += dword_F64E08 >> spinningInertia;
                break;
            }
            [[fallthrough]];
        case SpinFunction::R9:
            spinningInertia += 9;
            spin_speed -= dword_F64E08 >> spinningInertia;
            break;
        case SpinFunction::R8:
            spinningInertia += 8;
            spin_speed -= dword_F64E08 >> spinningInertia;
            break;
        case SpinFunction::None:
            break;
    }

    spinSpeed = std::clamp(spin_speed, kVehicleMinSpinSpeed, kVehicleMaxSpinSpeed);
    spin_speed = spinSpeed;
    spin_sprite += spinSpeed >> 8;
    // Note this actually increases the spin speed if going right!
    spin_speed -= spinSpeed >> carEntry->spinning_friction;
    Invalidate();
}

void Vehicle::UpdateAnimationAnimalFlying()
{
    if (animationState > 0)
    {
        animationState--;
        return;
    }

    if (animation_frame == 0)
    {
        auto trackType = GetTrackType();
        TileElement* trackElement = MapGetTrackElementAtOfTypeSeq(TrackLocation, trackType, 0);
        if (trackElement != nullptr && trackElement->AsTrack()->HasChain())
        {
            // start flapping, bird
            animation_frame = 1;
            animationState = 5;
            Invalidate();
        }
    }
    else
    {
        // continue flapping until reaching frame 0
        animation_frame = (animation_frame + 1) % 4;
        Invalidate();
    }
    // number of frames to skip before updating again
    constexpr std::array frameWaitTimes = { 5, 3, 5, 3 };
    animationState = frameWaitTimes[animation_frame];
}

/**
 * Get the frame of animation for the current animationState based on animation speed and animation frames
 */
static uint8_t GetTargetFrame(const CarEntry& carEntry, uint32_t animationState)
{
    if (carEntry.AnimationSpeed == 0)
        return 0;
    auto targetFrame = animationState / (carEntry.AnimationSpeed << 2);
    // mask of 0xFF
    targetFrame &= std::numeric_limits<uint8_t>::max();
    // multiply by number of frames. After the bitshift 8, the range will be 0 to AnimationFrames - 1
    targetFrame *= carEntry.AnimationFrames;
    return targetFrame >> std::numeric_limits<uint8_t>::digits;
}

/**
 * Compute the position that steam should be spawned
 */
static constexpr CoordsXYZ ComputeSteamOffset(int32_t height, int32_t length, VehiclePitch pitch, uint8_t yaw)
{
    uint8_t trueYaw = Entity::Yaw::YawTo64(yaw);
    auto offsets = Math::Trigonometry::PitchToDirectionVectorFromGeometry[EnumValue(pitch)];
    int32_t projectedRun = (offsets.x * length - offsets.y * height) / 256;
    int32_t projectedHeight = (offsets.x * height + offsets.y * length) / 256;
    return { Math::Trigonometry::ComputeXYVector(projectedRun, trueYaw), projectedHeight };
}

/**
 * Decide based on current frame and number of frames if a steam particle should be generated on this frame
 */
static bool ShouldMakeSteam(uint8_t targetFrame, uint8_t animationFrames)
{
    if (animationFrames < 1)
        return false;
    // steam is produced twice per wheel revolution
    return targetFrame == 0 || targetFrame == animationFrames / 2;
}

/**
 * Dummy function
 */
static void AnimateNone(Vehicle& vehicle, const CarEntry& carEntry)
{
    return;
}

/**
 * Animate the vehicle based on its speed
 */
static void AnimateSimpleVehicle(Vehicle& vehicle, const CarEntry& carEntry)
{
    vehicle.animationState += _vehicleVelocityF64E08;
    uint8_t targetFrame = GetTargetFrame(carEntry, vehicle.animationState);
    if (vehicle.animation_frame != targetFrame)
    {
        vehicle.animation_frame = targetFrame;
        vehicle.Invalidate();
    }
}

/**
 * Animate the vehicle based on its speed plus add steam particles
 */
static void AnimateSteamLocomotive(Vehicle& vehicle, const CarEntry& carEntry)
{
    vehicle.animationState += _vehicleVelocityF64E08;
    uint8_t targetFrame = GetTargetFrame(carEntry, vehicle.animationState);
    if (vehicle.animation_frame != targetFrame)
    {
        vehicle.animation_frame = targetFrame;
        if (ShouldMakeSteam(targetFrame, carEntry.AnimationFrames))
        {
            auto curRide = vehicle.GetRide();
            if (curRide != nullptr)
            {
                if (!RideHasStationShelter(*curRide)
                    || (vehicle.status != Vehicle::Status::movingToEndOfStation && vehicle.status != Vehicle::Status::arriving))
                {
                    CoordsXYZ steamOffset = ComputeSteamOffset(
                        carEntry.SteamEffect.Vertical, carEntry.SteamEffect.Longitudinal, vehicle.pitch, vehicle.Orientation);
                    SteamParticle::Create(CoordsXYZ(vehicle.x, vehicle.y, vehicle.z) + steamOffset);
                }
            }
        }
        vehicle.Invalidate();
    }
}

/**
 * Animate the vehicle based on its speed. Specialized animation with exactly 2 frames due to how peep animation works.
 */
static void AnimateSwanBoat(Vehicle& vehicle, const CarEntry& carEntry)
{
    // The animation of swan boats places frames at 0 and 2 instead of 0 and 1 like Water Tricycles due to the second
    // pair of peeps. The animation technically uses 4 frames, but ignores frames 1 and 3.
    vehicle.animationState += _vehicleVelocityF64E08;
    uint8_t targetFrame = GetTargetFrame(carEntry, vehicle.animationState) * 2;
    if (vehicle.animation_frame != targetFrame)
    {
        vehicle.animation_frame = targetFrame;
        vehicle.Invalidate();
    }
}

/**
 * Monorail Cycle animation only animates when a peep is present
 */
static void AnimateMonorailCycle(Vehicle& vehicle, const CarEntry& carEntry)
{
    if (vehicle.num_peeps != 0)
    {
        AnimateSimpleVehicle(vehicle, carEntry);
    }
}

/**
 * Observation tower animates at a constant speed continuously
 */
static void AnimateObservationTower(Vehicle& vehicle, const CarEntry& carEntry)
{
    if (vehicle.animationState <= 0xCCCC)
    {
        vehicle.animationState += carEntry.AnimationSpeed;
    }
    else
    {
        vehicle.animationState = 0;
        vehicle.animation_frame += 1;
        vehicle.animation_frame %= carEntry.AnimationFrames;
        vehicle.Invalidate();
    }
}
/**
 * seatRotation value of 4 translates to animationFrame value of 0. This function makes that true for any number of animation
 * frames
 */
static int16_t MultiDimensionTargetAngle(int16_t seatRotation, int16_t animationFrames)
{
    return ((seatRotation - 4) % animationFrames + animationFrames) % animationFrames;
}

/**
 * Multidimension targets a specific animation frame based on track
 */
static void AnimateMultiDimension(Vehicle& vehicle, const CarEntry& carEntry)
{
    if (vehicle.seat_rotation != vehicle.target_seat_rotation)
    {
        if (vehicle.animationState <= 0xCCCC)
        {
            vehicle.animationState += carEntry.AnimationSpeed;
        }
        else
        {
            vehicle.animationState = 0;

            if (vehicle.seat_rotation >= vehicle.target_seat_rotation)
                vehicle.seat_rotation--;
            else
                vehicle.seat_rotation++;

            int16_t targetSeatRotation = MultiDimensionTargetAngle(vehicle.seat_rotation, carEntry.AnimationFrames);
            if (targetSeatRotation != vehicle.animation_frame)
            {
                vehicle.animation_frame = targetSeatRotation;
                vehicle.Invalidate();
            }
        }
    }
}

/**
 * Animal Flying animates only on chainlift and in an unusual way. Made by Spacek531
 */
static void AnimateAnimalFlying(Vehicle& vehicle, const CarEntry& carEntry)
{
    vehicle.UpdateAnimationAnimalFlying();
    // makes animation play faster with vehicle speed
    uint8_t targetFrame = abs(_vehicleVelocityF64E08) >> carEntry.AnimationSpeed;
    vehicle.animationState = std::max(vehicle.animationState - targetFrame, 0u);
}

using AnimateFunction = void (*)(Vehicle& vehicle, const CarEntry& carEntry);

constexpr static const AnimateFunction AnimationFunctions[]{
    AnimateNone,          AnimateSimpleVehicle,  AnimateSteamLocomotive,  AnimateSwanBoat,
    AnimateMonorailCycle, AnimateMultiDimension, AnimateObservationTower, AnimateAnimalFlying,
};
static_assert(std::size(AnimationFunctions) == EnumValue(CarEntryAnimation::Count));

/**
 *
 *  rct2: 0x006D63D4
 */
void Vehicle::UpdateAdditionalAnimation()
{
    auto carEntry = Entry();
    if (carEntry == nullptr)
    {
        return;
    }
    if (carEntry->AnimationFrames == 0 || carEntry->animation >= CarEntryAnimation::Count)
        return;
    AnimationFunctions[EnumValue(carEntry->animation)](*this, *carEntry);
}
