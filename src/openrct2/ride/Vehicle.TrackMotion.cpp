/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Vehicle.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../audio/Audio.h"
#include "../core/Speed.hpp"
#include "../entity/EntityRegistry.h"
#include "../entity/EntityTweener.h"
#include "../scenario/Scenario.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"
#include "VehicleGeometry.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::TrackMetaData;
using namespace OpenRCT2::RideVehicle;

// External globals from Vehicle.cpp
extern Vehicle* gCurrentVehicle;
extern uint8_t _vehicleBreakdown;
extern StationIndex _vehicleStationIndex;
extern uint32_t _vehicleMotionTrackFlags;
extern int32_t _vehicleVelocityF64E08;
extern int32_t _vehicleVelocityF64E0C;
extern int32_t _vehicleUnkF64E10;
extern uint8_t _vehicleF64E2C;
extern Vehicle* _vehicleFrontVehicle;
extern CoordsXYZ _vehicleCurPosition;

// Constants
constexpr uint8_t kTrackSpeedShiftAmount = 16;
constexpr uint8_t kBoosterAccelerationShiftAmount = 16;
constexpr int16_t kVehicleMaxSpinSpeedWaterRide = 512;
constexpr int16_t kVehicleMinSpinSpeedWaterRide = -kVehicleMaxSpinSpeedWaterRide;

/**
 *
 *  rct2: 0x006DAB90
 */
void Vehicle::UpdateTrackMotionUpStopCheck() const
{
    const auto* carEntry = Entry();
    if (carEntry == nullptr)
    {
        return;
    }

    // No up stops (coaster types)
    if (carEntry->flags.has(CarEntryFlag::hasNoUpstopWheels))
    {
        if (!IsOnCoveredTrack())
        {
            auto gForces = GetGForces();
            gForces.lateralG = std::abs(gForces.lateralG);
            if (gForces.lateralG <= 150)
            {
                if (Geometry::getAccelerationFromPitch(pitch) < 0)
                {
                    if (gForces.verticalG > -40)
                    {
                        return;
                    }
                }
                else if (gForces.verticalG > -80)
                {
                    return;
                }
            }

            if (pitch != VehiclePitch::down60)
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_DERAILED;
            }
        }
    }
    else if (carEntry->flags.has(CarEntryFlag::hasNoUpstopWheelsBobsleigh))
    {
        // No up stops bobsleigh type
        if (!IsOnCoveredTrack())
        {
            auto gForces = GetGForces();

            if (Geometry::getAccelerationFromPitch(pitch) < 0)
            {
                if (gForces.verticalG > -45)
                {
                    return;
                }
            }
            else
            {
                if (gForces.verticalG > -80)
                {
                    return;
                }
            }

            if (pitch != VehiclePitch::down60 && pitch != VehiclePitch::down50)
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_DERAILED;
            }
        }
    }
}

/**
 * Modifies the train's velocity to match the block-brake fixed velocity.
 * This function must be called when the car is running through a non-stopping
 * state block-brake (precondition), which means that the block brake is acting
 * merely as a velocity regulator, in a closed state. When the brake is open, it
 * boosts the train to the speed limit
 */
void Vehicle::ApplyNonStopBlockBrake()
{
    if (velocity >= 0)
    {
        // If the vehicle is below the speed limit
        if (velocity <= kBlockBrakeBaseSpeed)
        {
            // Boost it to the fixed block brake speed
            velocity = kBlockBrakeBaseSpeed;
            acceleration = 0;
        }
        else if (velocity > (brake_speed << kTrackSpeedShiftAmount) + kBlockBrakeSpeedOffset)
        {
            velocity -= velocity >> 4;
            acceleration = 0;
        }
    }
}

/**
 *
 * Modifies the train's velocity influenced by a block brake
 */
void Vehicle::ApplyStopBlockBrake()
{
    // Slow it down till completely stop the car
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_BLOCK_BRAKE;
    acceleration = 0;
    // If the this is slow enough, stop it. If not, slow it down
    if (velocity <= 2.0_mph)
    {
        velocity = 0;
    }
    else
    {
        velocity -= velocity >> 3;
    }
}

void Vehicle::ApplyCableLiftBlockBrake(bool brakeClosed)
{
    // If we are already on the cable lift, ignore the brake
    if (status == Status::travellingCableLift)
        return;

    // Slow down if travelling faster than 4mph
    if (velocity > kBlockBrakeBaseSpeed)
    {
        velocity -= velocity >> 3;
        acceleration = 0;
    }
    // Once moving slow enough, keep moving forward until lined up with the catch car
    else if (track_progress <= 18)
    {
        velocity = kBlockBrakeBaseSpeed;
        acceleration = 0;
    }
    // Once in position, stop, and wait for the catch car
    if (velocity > 0 && track_progress >= 18)
    {
        velocity = 0;
        acceleration = 0;
        if (!brakeClosed)
            SetState(Status::waitingForCableLift, sub_state);
        else
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_BLOCK_BRAKE;
    }
}

/**
 *
 *  rct2: 0x006DAC43
 */
void Vehicle::CheckAndApplyBlockSectionStopSite()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    auto carEntry = Entry();
    if (carEntry == nullptr)
        return;

    // Is chair lift type
    if (carEntry->flags.has(CarEntryFlag::isChairlift))
    {
        velocity = _vehicleBreakdown == 0 ? 0 : curRide->speed << 16;
        acceleration = 0;
    }

    auto trackType = GetTrackType();

    TileElement* trackElement = MapGetTrackElementAtOfType(TrackLocation, trackType);

    if (trackElement == nullptr)
    {
        return;
    }

    switch (trackType)
    {
        case TrackElemType::blockBrakes:
            // Check if this brake is the start of a cable lift
            if (curRide->lifecycleFlags & RIDE_LIFECYCLE_CABLE_LIFT)
            {
                CoordsXYE track;
                int32_t zUnused;
                int32_t direction;
                if (TrackBlockGetNextFromZero(TrackLocation, *curRide, GetTrackDirection(), &track, &zUnused, &direction, false)
                    && track.element != nullptr && track.element->AsTrack()->HasCableLift())
                {
                    ApplyCableLiftBlockBrake(curRide->isBlockSectioned() && trackElement->AsTrack()->IsBrakeClosed());
                    break;
                }
            }
            [[fallthrough]];
        case TrackElemType::diagBlockBrakes:
            if (curRide->isBlockSectioned() && trackElement->AsTrack()->IsBrakeClosed())
                ApplyStopBlockBrake();
            else
                ApplyNonStopBlockBrake();

            break;
        case TrackElemType::endStation:
            if (trackElement->AsTrack()->IsBrakeClosed())
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_BLOCK_BRAKE;

            break;
        case TrackElemType::up25ToFlat:
        case TrackElemType::up60ToFlat:
        case TrackElemType::cableLiftHill:
        case TrackElemType::diagUp25ToFlat:
        case TrackElemType::diagUp60ToFlat:
            if (curRide->isBlockSectioned())
            {
                if (trackType == TrackElemType::cableLiftHill || trackElement->AsTrack()->HasChain())
                {
                    if (trackElement->AsTrack()->IsBrakeClosed())
                    {
                        ApplyStopBlockBrake();
                    }
                }
            }
            break;
        default:
            break;
    }
}

/**
 *
 *  rct2: 0x006DADAE
 */
void Vehicle::UpdateVelocity()
{
    int32_t nextVelocity = acceleration + velocity;
    if (flags.has(VehicleFlag::stoppedBySafetyCutout))
    {
        nextVelocity = 0;
    }
    if (flags.has(VehicleFlag::stoppedOnHoldingBrake))
    {
        if (vertical_drop_countdown > 0)
        {
            nextVelocity = 0;
            acceleration = 0;
            vertical_drop_countdown--;
        }
    }
    velocity = nextVelocity;

    _vehicleVelocityF64E08 = nextVelocity;
    _vehicleVelocityF64E0C = (nextVelocity >> 10) * 42;
}

static void BlockBrakesOpenPreviousSection(const Ride& ride, const CoordsXYZ& vehicleTrackLocation, TileElement* tileElement)
{
    CoordsXYZ location = vehicleTrackLocation;
    TrackElement* trackElement = TrackGetPreviousBlock(location, tileElement);
    if (trackElement == nullptr)
        return;

    SetBrakeClosedMultiTile(*trackElement, location, false);
    MapInvalidateElement(location, reinterpret_cast<TileElement*>(trackElement));

    auto trackType = trackElement->GetTrackType();
    if (trackType == TrackElemType::endStation)
    {
        Play3D(SoundId::blockBrakeClose, location);
    }
    else if (TrackTypeIsBlockBrakes(trackType))
    {
        Play3D(SoundId::blockBrakeClose, location);
        BlockBrakeSetLinkedBrakesClosed(location, *trackElement, false);
    }
}

/**
 *
 *  rct2: 0x006DB38B
 */
static PitchAndRoll PitchAndRollStart(bool useInvertedSprites, TileElement* tileElement)
{
    auto trackType = tileElement->AsTrack()->GetTrackType();
    const auto& ted = GetTrackElementDescriptor(trackType);
    return PitchAndRoll{ ted.definition.pitchStart, TrackGetActualBank3(useInvertedSprites, tileElement) };
}

void Vehicle::UpdateGoKartAttemptSwitchLanes()
{
    uint16_t probability = 0x8000;
    if (flags.has(VehicleFlag::currentlyColliding))
    {
        flags.unset(VehicleFlag::currentlyColliding);
    }
    else
    {
        probability = 0x0A3D;
    }
    if ((ScenarioRand() & 0xFFFF) <= probability)
    {
        // This changes "riding left" to "moving to right lane" and "riding right" to "moving to left lane".
        TrackSubposition = VehicleTrackSubposition{ static_cast<uint8_t>(static_cast<uint8_t>(TrackSubposition) + 2u) };
    }
}

static void vehicle_update_play_water_splash_sound()
{
    if (_vehicleVelocityF64E08 <= kBlockBrakeBaseSpeed)
    {
        return;
    }

    Play3D(SoundId::waterSplash, { _vehicleCurPosition.x, _vehicleCurPosition.y, _vehicleCurPosition.z });
}

/**
 *
 *  rct2: 0x006DB59E
 */
void Vehicle::UpdateHandleWaterSplash() const
{
    const auto* rideEntry = GetRideEntry();
    auto trackType = GetTrackType();

    if (!rideEntry->flags.has(RideEntryFlag::playSplashSound) && rideEntry->flags.has(RideEntryFlag::playSplashSoundSlide))
    {
        if (IsHead())
        {
            if (IsOnCoveredTrack())
            {
                Vehicle* nextVehicle = getGameState().entities.GetEntity<Vehicle>(next_vehicle_on_ride);
                if (nextVehicle == nullptr)
                    return;

                Vehicle* nextNextVehicle = getGameState().entities.GetEntity<Vehicle>(nextVehicle->next_vehicle_on_ride);
                if (nextNextVehicle == nullptr)
                    return;
                if (!nextNextVehicle->IsOnCoveredTrack())
                {
                    if (track_progress == 4)
                    {
                        vehicle_update_play_water_splash_sound();
                    }
                }
            }
        }
    }
    else
    {
        if (trackType == TrackElemType::down25ToFlat)
        {
            if (track_progress == 12)
            {
                vehicle_update_play_water_splash_sound();
            }
        }
    }
    if (IsHead())
    {
        if (trackType == TrackElemType::watersplash)
        {
            if (track_progress == 48)
            {
                vehicle_update_play_water_splash_sound();
            }
        }
    }
}

/**
 *
 *  rct2: 0x006DBF3E
 */
void Vehicle::Sub6DBF3E()
{
    const auto* carEntry = Entry();

    acceleration /= _vehicleUnkF64E10;
    if (TrackSubposition == VehicleTrackSubposition::ChairliftGoingBack)
    {
        return;
    }

    auto trackType = GetTrackType();
    const auto& ted = GetTrackElementDescriptor(trackType);
    if (!ted.sequences[0].flags.has(SequenceFlag::trackOrigin))
    {
        return;
    }

    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_3;

    TileElement* tileElement = nullptr;
    if (MapIsLocationValid(TrackLocation))
    {
        tileElement = MapGetTrackElementAtOfTypeSeq(TrackLocation, trackType, 0);
    }

    if (tileElement == nullptr)
    {
        return;
    }

    if (_vehicleStationIndex.IsNull())
    {
        _vehicleStationIndex = tileElement->AsTrack()->GetStationIndex();
    }

    if (trackType == TrackElemType::towerBase && this == gCurrentVehicle)
    {
        if (track_progress > 3 && !flags.has(VehicleFlag::poweredCarInReverse))
        {
            CoordsXYE output;
            int32_t outputZ, outputDirection;

            CoordsXYE input = { TrackLocation, tileElement };
            if (!TrackBlockGetNext(&input, &output, &outputZ, &outputDirection))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_12;
            }
        }

        if (track_progress <= 3)
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
        }
    }

    if (trackType != TrackElemType::endStation || this != gCurrentVehicle)
    {
        return;
    }

    uint16_t ax = track_progress;
    if (_vehicleVelocityF64E08 < 0)
    {
        if (ax <= 22)
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
        }
    }
    else
    {
        uint16_t cx = 17;
        if (carEntry->flags.has(CarEntryFlag::isChairlift))
        {
            cx = 6;
        }
        if (carEntry->flags.has(CarEntryFlag::isGoKart))
        {
            // Determine the stop positions for the karts. If in left lane it's further along the track than the right lane.
            // Since it's not possible to overtake when the race has ended, this does not check for overtake states (7 and
            // 8).
            cx = TrackSubposition == VehicleTrackSubposition::GoKartsRightLane ? 18 : 20;
        }

        if (ax > cx)
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
        }
    }
}

/**
 * Determine whether to use block brake speed or brake speed. If block brake is closed or no block brake present, use the
 * brake's speed; if block brake is open, use maximum of brake speed or block brake speed.
 */
uint8_t Vehicle::ChooseBrakeSpeed() const
{
    if (!TrackTypeIsBrakes(GetTrackType()))
        return brake_speed;
    auto trackElement = MapGetTrackElementAtOfTypeSeq(TrackLocation, GetTrackType(), 0);
    if (trackElement != nullptr)
    {
        if (trackElement->AsTrack()->IsBrakeClosed())
            return brake_speed;
        else
            return std::max<uint8_t>(brake_speed, BlockBrakeSpeed);
    }
    return brake_speed;
}

/**
 * Populate the vehicle's brake_speed and BlockBrakeSpeed values.
 */
void Vehicle::PopulateBrakeSpeed(const CoordsXYZ& vehicleTrackLocation, TrackElement& brake)
{
    auto trackSpeed = brake.GetBrakeBoosterSpeed();
    brake_speed = trackSpeed;
    if (!TrackTypeIsBrakes(brake.GetTrackType()))
    {
        BlockBrakeSpeed = trackSpeed;
        return;
    }
    // As soon as feasible, encode block brake speed into track element so the lookforward can be skipped here.

    CoordsXYE output = CoordsXYE(vehicleTrackLocation.x, vehicleTrackLocation.y, reinterpret_cast<TileElement*>(&brake));
    int32_t outputZ = vehicleTrackLocation.z;
    uint16_t timeoutCount = 256;
    do
    {
        if (TrackTypeIsBlockBrakes(output.element->AsTrack()->GetTrackType()))
        {
            BlockBrakeSpeed = output.element->AsTrack()->GetBrakeBoosterSpeed();
            return;
        }
        if (!TrackTypeIsBrakes(output.element->AsTrack()->GetTrackType()))
        {
            break;
        }
        timeoutCount--;
    } while (TrackBlockGetNext(&output, &output, &outputZ, nullptr) && timeoutCount);

    // If block brake is not found, use the track's speed
    BlockBrakeSpeed = trackSpeed;
}

/**
 *
 *  rct2: 0x006DB08C
 */
bool Vehicle::UpdateTrackMotionForwardsGetNewTrack(
    TrackElemType trackType, const Ride& curRide, const RideObjectEntry& rideEntry)
{
    CoordsXYZD location = {};

    auto pitchAndRollEnd = TrackPitchAndRollEnd(trackType);
    TileElement* tileElement = MapGetTrackElementAtOfTypeSeq(TrackLocation, trackType, 0);

    if (tileElement == nullptr)
    {
        return false;
    }

    if (trackType == TrackElemType::cableLiftHill && this == gCurrentVehicle)
    {
        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_11;
    }

    if (tileElement->AsTrack()->IsBlockStart())
    {
        if (next_vehicle_on_train.IsNull())
        {
            SetBrakeClosedMultiTile(*tileElement->AsTrack(), TrackLocation, true);
            if (TrackTypeIsBlockBrakes(trackType) || trackType == TrackElemType::endStation)
            {
                if (!rideEntry.Cars[0].flags.has(CarEntryFlag::isPowered))
                {
                    Play3D(SoundId::blockBrakeRelease, TrackLocation);
                }
            }
            MapInvalidateElement(TrackLocation, tileElement);
            BlockBrakesOpenPreviousSection(curRide, TrackLocation, tileElement);
            if (TrackTypeIsBlockBrakes(trackType))
            {
                BlockBrakeSetLinkedBrakesClosed(TrackLocation, *tileElement->AsTrack(), true);
            }
        }
    }

    // Change from original: this used to check if the vehicle allowed doors.
    UpdateSceneryDoor();

    bool isGoingBack = false;
    switch (TrackSubposition)
    {
        case VehicleTrackSubposition::ChairliftGoingBack:
        case VehicleTrackSubposition::ChairliftEndBullwheel:
            TrackSubposition = VehicleTrackSubposition::ChairliftGoingBack;
            isGoingBack = true;
            break;
        case VehicleTrackSubposition::ChairliftStartBullwheel:
            TrackSubposition = VehicleTrackSubposition::ChairliftGoingOut;
            break;
        case VehicleTrackSubposition::GoKartsMovingToRightLane:
            TrackSubposition = VehicleTrackSubposition::GoKartsRightLane;
            break;
        case VehicleTrackSubposition::GoKartsMovingToLeftLane:
            TrackSubposition = VehicleTrackSubposition::GoKartsLeftLane;
            break;
        default:
            break;
    }

    if (isGoingBack)
    {
        TrackBeginEnd trackBeginEnd;
        if (!TrackBlockGetPrevious({ TrackLocation, tileElement }, &trackBeginEnd))
        {
            return false;
        }
        location.x = trackBeginEnd.begin_x;
        location.y = trackBeginEnd.begin_y;
        location.z = trackBeginEnd.begin_z;
        location.direction = trackBeginEnd.begin_direction;
        tileElement = trackBeginEnd.begin_element;
    }
    else
    {
        {
            int32_t curZ, direction;
            CoordsXYE xyElement = { TrackLocation, tileElement };
            if (!TrackBlockGetNext(&xyElement, &xyElement, &curZ, &direction))
            {
                return false;
            }
            tileElement = xyElement.element;
            location = { xyElement, curZ, static_cast<Direction>(direction) };
        }
        if (tileElement->AsTrack()->GetTrackType() == TrackElemType::leftReverser
            || tileElement->AsTrack()->GetTrackType() == TrackElemType::rightReverser)
        {
            if (IsHead() && velocity <= 3.0_mph)
            {
                velocity = 0;
            }
        }

        if (PitchAndRollStart(flags.has(VehicleFlag::carIsInverted), tileElement) != pitchAndRollEnd)
        {
            return false;
        }

        // Update VehicleFlags::CarIsInverted flag
        const auto previousCarIsInverted = flags.has(VehicleFlag::carIsInverted);
        flags.unset(VehicleFlag::carIsInverted);
        {
            auto rideType = ::GetRide(tileElement->AsTrack()->GetRideIndex())->type;
            if (GetRideTypeDescriptor(rideType).flags.has(RtdFlag::hasInvertedVariant))
            {
                if (tileElement->AsTrack()->IsInverted())
                {
                    flags.set(VehicleFlag::carIsInverted);
                }
            }
            if (previousCarIsInverted != flags.has(VehicleFlag::carIsInverted))
            {
                EntityTweener::Get().RemoveEntity(this);
            }
        }
    }

    const int32_t previousTrackHeight = TrackLocation.z;
    TrackLocation = location;

    // TODO check if getting the vehicle entry again is necessary
    const auto* carEntry = Entry();
    if (carEntry == nullptr)
    {
        return false;
    }
    if (carEntry->flags.has(CarEntryFlag::isGoKart) && TrackSubposition < VehicleTrackSubposition::GoKartsMovingToRightLane)
    {
        trackType = tileElement->AsTrack()->GetTrackType();
        if (trackType == TrackElemType::flat || trackType == TrackElemType::leftQuarterTurn3Tiles
            || trackType == TrackElemType::rightQuarterTurn3Tiles || trackType == TrackElemType::leftQuarterTurn5Tiles
            || trackType == TrackElemType::rightQuarterTurn5Tiles || trackType == TrackElemType::leftEighthToDiag
            || trackType == TrackElemType::rightEighthToDiag || trackType == TrackElemType::leftEighthToOrthogonal
            || trackType == TrackElemType::rightEighthToOrthogonal || trackType == TrackElemType::diagFlat
            || trackType == TrackElemType::sBendLeft || trackType == TrackElemType::sBendRight
            || ((curRide.lifecycleFlags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING) && tileElement->AsTrack()->IsStation()))
        {
            UpdateGoKartAttemptSwitchLanes();
        }
    }

    if (TrackSubposition >= VehicleTrackSubposition::ChairliftGoingOut
        && TrackSubposition <= VehicleTrackSubposition::ChairliftStartBullwheel)
    {
        TileCoordsXYZ curLocation{ TrackLocation };

        if (curLocation == curRide.chairliftBullwheelLocation[1])
        {
            TrackSubposition = VehicleTrackSubposition::ChairliftEndBullwheel;
        }
        else if (curLocation == curRide.chairliftBullwheelLocation[0])
        {
            TrackSubposition = VehicleTrackSubposition::ChairliftStartBullwheel;
        }
    }

    // Loc6DB500
    // Update VehicleFlags::OnLiftHill
    flags.unset(VehicleFlag::onLiftHill);
    if (tileElement->AsTrack()->HasChain())
    {
        flags.set(VehicleFlag::onLiftHill);
    }

    trackType = tileElement->AsTrack()->GetTrackType();
    if (trackType != TrackElemType::brakes)
    {
        target_seat_rotation = tileElement->AsTrack()->GetSeatRotation();
    }
    SetTrackDirection(location.direction);
    SetTrackType(trackType);
    PopulateBrakeSpeed(TrackLocation, *tileElement->AsTrack());
    if (flags.has(VehicleFlag::stoppedOnHoldingBrake) && vertical_drop_countdown <= 0)
    {
        flags.unset(VehicleFlag::stoppedOnHoldingBrake);
    }
    if (trackType == TrackElemType::onRidePhoto)
    {
        tileElement->AsTrack()->SetPhotoTimeout();
        MapAnimations::CreateTemporary(TrackLocation, MapAnimations::TemporaryType::onRidePhoto);
    }
    if (trackType == TrackElemType::rotationControlToggle)
    {
        flags.flip(VehicleFlag::spinningIsLocked);
    }
    // Change from original: this used to check if the vehicle allowed doors.
    UpdateSceneryDoorBackwards();
    UpdateLandscapeDoors(previousTrackHeight);

    return true;
}

/**
 *
 *  rct2: 0x006DAEB9
 */
bool Vehicle::UpdateTrackMotionForwards(const CarEntry* carEntry, const Ride& curRide, const RideObjectEntry& rideEntry)
{
    EntityId otherVehicleIndex = EntityId::GetNull();
    while (true)
    {
        auto trackType = GetTrackType();
        if (trackType == TrackElemType::heartLineTransferUp || trackType == TrackElemType::heartLineTransferDown)
        {
            if (track_progress == 80)
            {
                vehicle_type ^= 1;
                carEntry = Entry();
            }
            if (_vehicleVelocityF64E08 >= 0x40000)
            {
                acceleration = -_vehicleVelocityF64E08 * 8;
            }
            else if (_vehicleVelocityF64E08 < 0x20000)
            {
                acceleration = 0x50000;
            }
        }
        else if (TrackTypeIsBrakes(trackType))
        {
            bool hasBrakesFailure = curRide.lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN
                && curRide.breakdownReasonPending == BREAKDOWN_BRAKES_FAILURE;
            if (!hasBrakesFailure || curRide.mechanicStatus == MechanicStatus::hasFixedStationBrakes)
            {
                auto brakeSpeed = ChooseBrakeSpeed() << kTrackSpeedShiftAmount;

                if ((brakeSpeed) < _vehicleVelocityF64E08)
                {
                    acceleration = -_vehicleVelocityF64E08 * 16;
                }
                else if (!(getGameState().currentTicks & 0x0F))
                {
                    if (_vehicleF64E2C == 0)
                    {
                        _vehicleF64E2C++;
                        Play3D(SoundId::brakeRelease, { x, y, z });
                    }
                }
            }
        }
        else if (TrackTypeIsBooster(trackType))
        {
            auto boosterSpeed = GetUnifiedBoosterSpeed(curRide.type, brake_speed) << kTrackSpeedShiftAmount;
            if (boosterSpeed > _vehicleVelocityF64E08)
            {
                acceleration = GetRideTypeDescriptor(curRide.type).LegacyBoosterSettings.BoosterAcceleration
                    << kBoosterAccelerationShiftAmount;
            }
        }
        else if (rideEntry.flags.has(RideEntryFlag::riderControlsSpeed) && num_peeps > 0)
        {
            acceleration += CalculateRiderBraking();
        }

        if ((trackType == TrackElemType::flat && curRide.getRideTypeDescriptor().flags.has(RtdFlag::hasLsmBehaviourOnFlat))
            || (trackType == TrackElemType::poweredLift))
        {
            acceleration = GetRideTypeDescriptor(curRide.type).LegacyBoosterSettings.PoweredLiftAcceleration
                << kBoosterAccelerationShiftAmount;
        }
        if (trackType == TrackElemType::brakeForDrop)
        {
            if (IsHead())
            {
                if (!flags.has(VehicleFlag::stoppedOnHoldingBrake))
                {
                    if (track_progress >= 8)
                    {
                        acceleration = -_vehicleVelocityF64E08 * 16;
                        if (track_progress >= 24)
                        {
                            flags.set(VehicleFlag::stoppedOnHoldingBrake);
                            vertical_drop_countdown = 90;
                        }
                    }
                }
            }
        }
        if (trackType == TrackElemType::logFlumeReverser)
        {
            if (track_progress != 16 || velocity < 4.0_mph)
            {
                if (track_progress == 32)
                {
                    vehicle_type = carEntry->ReversedCarIndex;
                    carEntry = Entry();
                }
            }
            else
            {
                track_progress += 17;
            }
        }

        uint16_t newTrackProgress = track_progress + 1;

        // Track Total Progress is in the two bytes before the move info list
        uint16_t trackTotalProgress = GetTrackProgress();
        if (newTrackProgress >= trackTotalProgress)
        {
            UpdateCrossings();

            if (!UpdateTrackMotionForwardsGetNewTrack(trackType, curRide, rideEntry))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                _vehicleVelocityF64E0C -= remaining_distance + 1;
                remaining_distance = -1;
                return false;
            }
            newTrackProgress = 0;
        }

        track_progress = newTrackProgress;
        UpdateHandleWaterSplash();

        // Loc6DB706
        const auto moveInfo = GetMoveInfo();
        trackType = GetTrackType();
        {
            auto nextVehiclePosition = TrackLocation
                + CoordsXYZ{ moveInfo->x, moveInfo->y,
                             moveInfo->z + GetRideTypeDescriptor(curRide.type).Heights.VehicleZOffset };

            bool useReverserDistance = TrackSubposition == VehicleTrackSubposition::ReverserRCFrontBogie
                && (trackType == TrackElemType::leftReverser || trackType == TrackElemType::rightReverser)
                && track_progress >= 30 && track_progress <= 66;

            // Loc6DB8A5
            remaining_distance -= Geometry::getTranslationDistance(
                nextVehiclePosition - _vehicleCurPosition, useReverserDistance);

            if (TrackSubposition == VehicleTrackSubposition::ReverserRCRearBogie
                && (trackType == TrackElemType::leftReverser || trackType == TrackElemType::rightReverser)
                && track_progress == 96)
            {
                ReverseReverserCar();

                const VehicleInfo* moveInfo2 = GetMoveInfo();
                nextVehiclePosition.x = x + moveInfo2->x;
                nextVehiclePosition.y = y + moveInfo2->y;
            }

            _vehicleCurPosition = nextVehiclePosition;
            Orientation = moveInfo->yaw;
            roll = moveInfo->roll;
            pitch = moveInfo->pitch;

            if (carEntry->flags.has(CarEntryFlag::useWoodenWildMouseSwing) && moveInfo->pitch != VehiclePitch::flat)
            {
                SwingSprite = 0;
                SwingPosition = 0;
                SwingSpeed = 0;
            }

            // this == frontVehicle
            if (this == _vehicleFrontVehicle)
            {
                if (_vehicleVelocityF64E08 >= 0)
                {
                    otherVehicleIndex = prev_vehicle_on_ride;
                    if (UpdateMotionCollisionDetection(nextVehiclePosition, &otherVehicleIndex))
                    {
                        _vehicleVelocityF64E0C -= remaining_distance + 1;
                        remaining_distance = -1;

                        // Might need to be bp rather than this, but hopefully not
                        auto otherVeh = getGameState().entities.GetEntity<Vehicle>(otherVehicleIndex);
                        if (otherVeh == nullptr)
                        {
                            // This can never happen as prev_vehicle_on_ride will always be set to a vehicle
                            LOG_ERROR("Failed to get next vehicle during update!");
                            return true;
                        }
                        auto head = otherVeh->TrainHead();

                        auto velocityDelta = abs(velocity - head->velocity);
                        if (!rideEntry.flags.has(RideEntryFlag::disableCollisionCrashes))
                        {
                            if (velocityDelta > 14.0_mph)
                            {
                                if (!carEntry->flags.has(CarEntryFlag::useBoatHireCollisionDetection))
                                {
                                    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION;
                                }
                            }
                        }

                        if (carEntry->flags.has(CarEntryFlag::isGoKart))
                        {
                            velocity -= velocity >> 2;
                        }
                        else
                        {
                            int32_t newHeadVelocity = velocity >> 1;
                            velocity = head->velocity >> 1;
                            head->velocity = newHeadVelocity;
                        }
                        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_1;
                        return false;
                    }
                }
            }
        }

        // Loc6DB928
        if (remaining_distance < 0x368A)
        {
            return true;
        }

        acceleration += Geometry::getAccelerationFromPitch(pitch);
        _vehicleUnkF64E10++;
    }
}

static PitchAndRoll PitchAndRollEnd(
    const Ride& curRide, bool useInvertedSprites, TrackElemType trackType, TileElement* tileElement)
{
    bool isInverted = useInvertedSprites ^ tileElement->AsTrack()->IsInverted();
    const auto& ted = GetTrackElementDescriptor(trackType);
    return { ted.definition.pitchEnd, TrackGetActualBank2(curRide.type, isInverted, ted.definition.rollEnd) };
}

/**
 *
 *  rct2: 0x006DBAA6
 */
bool Vehicle::UpdateTrackMotionBackwardsGetNewTrack(TrackElemType trackType, const Ride& curRide, uint16_t* progress)
{
    auto pitchAndRollStart = TrackPitchAndRollStart(trackType);
    TileElement* tileElement = MapGetTrackElementAtOfTypeSeq(TrackLocation, trackType, 0);

    if (tileElement == nullptr)
        return false;

    bool nextTileBackwards = true;
    int32_t direction = 0;
    // Loc6DBB08:;
    auto trackPos = CoordsXYZ{ TrackLocation.x, TrackLocation.y, 0 };

    switch (TrackSubposition)
    {
        case VehicleTrackSubposition::ChairliftEndBullwheel:
            TrackSubposition = VehicleTrackSubposition::ChairliftGoingOut;
            break;
        case VehicleTrackSubposition::GoKartsMovingToRightLane:
            TrackSubposition = VehicleTrackSubposition::GoKartsLeftLane;
            break;
        case VehicleTrackSubposition::GoKartsMovingToLeftLane:
            TrackSubposition = VehicleTrackSubposition::GoKartsRightLane;
            break;
        case VehicleTrackSubposition::ChairliftGoingBack:
        case VehicleTrackSubposition::ChairliftStartBullwheel:
            TrackSubposition = VehicleTrackSubposition::ChairliftGoingBack;
            nextTileBackwards = false;
            break;
        default:
            break;
    }

    if (nextTileBackwards)
    {
        // Loc6DBB7E:;
        TrackBeginEnd trackBeginEnd;
        if (!TrackBlockGetPrevious({ trackPos, tileElement }, &trackBeginEnd))
        {
            return false;
        }
        tileElement = trackBeginEnd.begin_element;

        trackType = tileElement->AsTrack()->GetTrackType();
        if (trackType == TrackElemType::leftReverser || trackType == TrackElemType::rightReverser)
        {
            return false;
        }

        if (PitchAndRollEnd(curRide, flags.has(VehicleFlag::carIsInverted), trackType, tileElement) != pitchAndRollStart)
        {
            return false;
        }

        // Update VehicleFlags::CarIsInverted
        const auto previousCarIsInverted = flags.has(VehicleFlag::carIsInverted);
        flags.unset(VehicleFlag::carIsInverted);
        if (GetRideTypeDescriptor(curRide.type).flags.has(RtdFlag::hasInvertedVariant))
        {
            if (tileElement->AsTrack()->IsInverted())
            {
                flags.set(VehicleFlag::carIsInverted);
            }
            if (previousCarIsInverted != flags.has(VehicleFlag::carIsInverted))
            {
                EntityTweener::Get().RemoveEntity(this);
            }
        }

        trackPos = { trackBeginEnd.begin_x, trackBeginEnd.begin_y, trackBeginEnd.begin_z };
        direction = trackBeginEnd.begin_direction;
    }
    else
    {
        // Loc6DBB4F:;
        CoordsXYE input;
        CoordsXYE output;
        int32_t outputZ{};

        input.x = trackPos.x;
        input.y = trackPos.y;
        input.element = tileElement;
        if (!TrackBlockGetNext(&input, &output, &outputZ, &direction))
        {
            return false;
        }
        tileElement = output.element;
        trackPos = { output, outputZ };
    }

    // Loc6DBC3B:
    TrackLocation = trackPos;

    if (TrackSubposition >= VehicleTrackSubposition::ChairliftGoingOut
        && TrackSubposition <= VehicleTrackSubposition::ChairliftStartBullwheel)
    {
        TileCoordsXYZ curLocation{ TrackLocation };

        if (curLocation == curRide.chairliftBullwheelLocation[1])
        {
            TrackSubposition = VehicleTrackSubposition::ChairliftEndBullwheel;
        }
        else if (curLocation == curRide.chairliftBullwheelLocation[0])
        {
            TrackSubposition = VehicleTrackSubposition::ChairliftStartBullwheel;
        }
    }

    if (tileElement->AsTrack()->HasChain())
    {
        if (_vehicleVelocityF64E08 < 0)
        {
            if (next_vehicle_on_train.IsNull())
            {
                trackType = tileElement->AsTrack()->GetTrackType();
                const auto& ted = GetTrackElementDescriptor(trackType);
                if (!ted.flags.has(TrackElementFlag::down))
                {
                    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_9;
                }
            }
            flags.set(VehicleFlag::onLiftHill);
        }
    }
    else
    {
        if (flags.has(VehicleFlag::onLiftHill))
        {
            flags.unset(VehicleFlag::onLiftHill);
            if (next_vehicle_on_train.IsNull())
            {
                if (_vehicleVelocityF64E08 < 0)
                {
                    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_8;
                }
            }
        }
    }

    trackType = tileElement->AsTrack()->GetTrackType();
    if (trackType != TrackElemType::brakes)
    {
        target_seat_rotation = tileElement->AsTrack()->GetSeatRotation();
    }
    direction &= 3;
    SetTrackType(trackType);
    SetTrackDirection(direction);
    PopulateBrakeSpeed(TrackLocation, *tileElement->AsTrack());
    if (flags.has(VehicleFlag::stoppedOnHoldingBrake) && vertical_drop_countdown <= 0)
    {
        flags.unset(VehicleFlag::stoppedOnHoldingBrake);
    }
    // There are two bytes before the move info list
    uint16_t trackTotalProgress = GetTrackProgress();
    *progress = trackTotalProgress - 1;
    return true;
}

/**
 *
 *  rct2: 0x006DBA33
 */
bool Vehicle::UpdateTrackMotionBackwards(const CarEntry* carEntry, const Ride& curRide, const RideObjectEntry& rideEntry)
{
    EntityId otherVehicleIndex = EntityId::GetNull();

    while (true)
    {
        auto trackType = GetTrackType();
        if (trackType == TrackElemType::flat && curRide.getRideTypeDescriptor().flags.has(RtdFlag::hasLsmBehaviourOnFlat))
        {
            int32_t unkVelocity = _vehicleVelocityF64E08;
            if (unkVelocity < -524288)
            {
                unkVelocity = abs(unkVelocity);
                acceleration = unkVelocity * 2;
            }
        }

        if (TrackTypeIsBrakes(trackType))
        {
            auto brakeSpeed = ChooseBrakeSpeed();

            if (-(brakeSpeed << kTrackSpeedShiftAmount) > _vehicleVelocityF64E08)
            {
                acceleration = _vehicleVelocityF64E08 * -16;
            }
        }

        uint16_t newTrackProgress = track_progress - 1;
        if (newTrackProgress == 0xFFFF)
        {
            UpdateCrossings();

            if (!UpdateTrackMotionBackwardsGetNewTrack(trackType, curRide, &newTrackProgress))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                _vehicleVelocityF64E0C -= remaining_distance - 0x368A;
                remaining_distance = 0x368A;
                return false;
            }
        }

        // Loc6DBD42
        track_progress = newTrackProgress;
        {
            const VehicleInfo* moveInfo = GetMoveInfo();
            auto nextVehiclePosition = TrackLocation
                + CoordsXYZ{ moveInfo->x, moveInfo->y,
                             moveInfo->z + GetRideTypeDescriptor(curRide.type).Heights.VehicleZOffset };

            remaining_distance += Geometry::getTranslationDistance(nextVehiclePosition - _vehicleCurPosition, false);

            _vehicleCurPosition = nextVehiclePosition;
            Orientation = moveInfo->yaw;
            roll = moveInfo->roll;
            pitch = moveInfo->pitch;

            if (carEntry->flags.has(CarEntryFlag::useWoodenWildMouseSwing) && pitch != VehiclePitch::flat)
            {
                SwingSprite = 0;
                SwingPosition = 0;
                SwingSpeed = 0;
            }

            if (this == _vehicleFrontVehicle)
            {
                if (_vehicleVelocityF64E08 < 0)
                {
                    otherVehicleIndex = next_vehicle_on_ride;
                    if (UpdateMotionCollisionDetection(nextVehiclePosition, &otherVehicleIndex))
                    {
                        _vehicleVelocityF64E0C -= remaining_distance - 0x368A;
                        remaining_distance = 0x368A;

                        Vehicle* v3 = getGameState().entities.GetEntity<Vehicle>(otherVehicleIndex);
                        Vehicle* v4 = gCurrentVehicle;
                        if (v3 == nullptr)
                        {
                            return false;
                        }

                        if (!rideEntry.flags.has(RideEntryFlag::disableCollisionCrashes))
                        {
                            if (abs(v4->velocity - v3->velocity) > 14.0_mph)
                            {
                                if (!carEntry->flags.has(CarEntryFlag::useBoatHireCollisionDetection))
                                {
                                    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION;
                                }
                            }
                        }

                        if (carEntry->flags.has(CarEntryFlag::isGoKart))
                        {
                            velocity -= velocity >> 2;
                            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_2;
                        }
                        else
                        {
                            int32_t v3Velocity = v3->velocity;
                            v3->velocity = v4->velocity >> 1;
                            v4->velocity = v3Velocity >> 1;
                            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_2;
                        }

                        return false;
                    }
                }
            }
        }

        // Loc6DBE3F
        if (remaining_distance >= 0)
        {
            return true;
        }
        acceleration += Geometry::getAccelerationFromPitch(pitch);
        _vehicleUnkF64E10++;
    }
}

static constexpr int32_t GetAccelerationDecrease2(const int32_t velocity, const int32_t totalMass)
{
    int32_t accelerationDecrease2 = velocity >> 8;
    accelerationDecrease2 *= accelerationDecrease2;
    if (velocity < 0)
    {
        accelerationDecrease2 = -accelerationDecrease2;
    }
    accelerationDecrease2 >>= 4;
    // OpenRCT2: vehicles from different track types can have  0 mass.
    if (totalMass != 0)
    {
        return accelerationDecrease2 / totalMass;
    }

    return accelerationDecrease2;
}

/**
 *
 *  rct2: 0x006DC1E4
 */
static uint8_t modified_speed(TrackElemType trackType, VehicleTrackSubposition trackSubposition, uint8_t speed)
{
    enum
    {
        FULL_SPEED,
        THREE_QUARTER_SPEED,
        HALF_SPEED
    };

    uint8_t speedModifier = FULL_SPEED;

    if (trackType == TrackElemType::leftQuarterTurn1Tile)
    {
        speedModifier = (trackSubposition == VehicleTrackSubposition::GoKartsLeftLane) ? HALF_SPEED : THREE_QUARTER_SPEED;
    }
    else if (trackType == TrackElemType::rightQuarterTurn1Tile)
    {
        speedModifier = (trackSubposition == VehicleTrackSubposition::GoKartsRightLane) ? HALF_SPEED : THREE_QUARTER_SPEED;
    }

    switch (speedModifier)
    {
        case HALF_SPEED:
            return speed >> 1;
        case THREE_QUARTER_SPEED:
            return speed - (speed >> 2);
    }
    return speed;
}

int32_t Vehicle::UpdateTrackMotionPoweredRideAcceleration(
    const CarEntry* carEntry, uint32_t totalMass, const int32_t curAcceleration)
{
    if (carEntry->flags.has(CarEntryFlag::isPoweredRideWithUnrestrictedGravity))
    {
        if (velocity > (speed * 0x4000))
        {
            // Same code as none powered rides
            if (curAcceleration <= 0 && curAcceleration >= -500 && velocity <= 0.5_mph)
            {
                return curAcceleration + 400;
            }
            return curAcceleration;
        }
    }
    uint8_t modifiedSpeed = modified_speed(GetTrackType(), TrackSubposition, speed);
    int32_t poweredAcceleration = modifiedSpeed << 14;
    int32_t quarterForce = (modifiedSpeed * totalMass) >> 2;
    if (flags.has(VehicleFlag::poweredCarInReverse))
    {
        poweredAcceleration = -poweredAcceleration;
    }
    poweredAcceleration -= velocity;
    poweredAcceleration *= powered_acceleration << 1;
    if (quarterForce != 0)
    {
        poweredAcceleration /= quarterForce;
    }

    if (carEntry->flags.has(CarEntryFlag::isLift))
    {
        poweredAcceleration *= 4;
    }

    if (carEntry->flags.has(CarEntryFlag::isWaterRide))
    {
        if (poweredAcceleration < 0)
        {
            poweredAcceleration >>= 4;
        }

        if (carEntry->flags.has(CarEntryFlag::hasSpinning))
        {
            spin_speed = std::clamp(spin_speed, kVehicleMinSpinSpeedWaterRide, kVehicleMaxSpinSpeedWaterRide);
        }

        if (pitch != VehiclePitch::flat)
        {
            if (poweredAcceleration < 0)
            {
                poweredAcceleration = 0;
            }

            if (carEntry->flags.has(CarEntryFlag::hasSpinning))
            {
                // If the vehicle is on the up slope kill the spin speedModifier
                if (pitch == VehiclePitch::up25)
                {
                    spin_speed = 0;
                }
            }
            return curAcceleration + poweredAcceleration;
        }
    }

    if (std::abs(velocity) <= 1.0_mph)
    {
        return poweredAcceleration;
    }

    return curAcceleration + poweredAcceleration;
}

void Vehicle::UpdateTrackMotionPreUpdate(
    Vehicle& car, const Ride& curRide, const RideObjectEntry& rideEntry, const CarEntry* carEntry)
{
    if (carEntry->flags.has(CarEntryFlag::hasSwinging))
    {
        car.UpdateSwingingCar();
    }
    if (carEntry->flags.has(CarEntryFlag::hasSpinning))
    {
        car.UpdateSpinningCar();
    }
    if (carEntry->flags.hasAny(CarEntryFlag::hasVehicleAnimation, CarEntryFlag::hasRiderAnimation))
    {
        car.UpdateAdditionalAnimation();
    }
    car.acceleration = Geometry::getAccelerationFromPitch(car.pitch);
    _vehicleUnkF64E10 = 1;

    if (!car.flags.has(VehicleFlag::moveSingleCar))
    {
        car.remaining_distance += _vehicleVelocityF64E0C;
    }

    car.sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
    _vehicleCurPosition.x = car.x;
    _vehicleCurPosition.y = car.y;
    _vehicleCurPosition.z = car.z;
    car.Invalidate();

    while (true)
    {
        if (car.remaining_distance < 0)
        {
            // Backward loop
            if (car.UpdateTrackMotionBackwards(carEntry, curRide, rideEntry))
            {
                break;
            }

            if (car.remaining_distance < 0x368A)
            {
                break;
            }
            car.acceleration += Geometry::getAccelerationFromPitch(car.pitch);
            _vehicleUnkF64E10++;
            continue;
        }
        if (car.remaining_distance < 0x368A)
        {
            // Location found
            return;
        }
        if (car.UpdateTrackMotionForwards(carEntry, curRide, rideEntry))
        {
            break;
        }

        if (car.remaining_distance >= 0)
        {
            break;
        }
        car.acceleration = Geometry::getAccelerationFromPitch(car.pitch);
        _vehicleUnkF64E10++;
    }
    // Loc6DBF20
    car.MoveTo(_vehicleCurPosition);
}

/**
 *
 *  rct2: 0x006DAB4C
 */
int32_t Vehicle::UpdateTrackMotion(int32_t* outStation)
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return 0;

    const auto* rideEntry = GetRideEntry();
    const auto* carEntry = Entry();

    if (carEntry == nullptr)
    {
        return 0;
    }

    if (carEntry->flags.has(CarEntryFlag::isMiniGolf))
    {
        return UpdateTrackMotionMiniGolf(outStation);
    }

    _vehicleF64E2C = 0;
    gCurrentVehicle = this;
    _vehicleMotionTrackFlags = 0;
    _vehicleStationIndex = StationIndex::GetNull();

    UpdateTrackMotionUpStopCheck();
    CheckAndApplyBlockSectionStopSite();
    UpdateVelocity();

    Vehicle* vehicle = this;
    if (_vehicleVelocityF64E08 < 0 && !vehicle->flags.has(VehicleFlag::moveSingleCar))
    {
        vehicle = vehicle->TrainTail();
    }
    // This will be the front vehicle even when traveling
    // backwards.
    _vehicleFrontVehicle = vehicle;

    auto spriteId = vehicle->Id;
    while (!spriteId.IsNull())
    {
        Vehicle* car = getGameState().entities.GetEntity<Vehicle>(spriteId);
        if (car == nullptr)
        {
            break;
        }
        carEntry = car->Entry();
        if (carEntry != nullptr)
        {
            UpdateTrackMotionPreUpdate(*car, *curRide, *rideEntry, carEntry);
        }

        car->Sub6DBF3E();

        // Loc6DC0F7
        if (car->flags.has(VehicleFlag::onLiftHill))
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL;
        }
        if (car->flags.has(VehicleFlag::moveSingleCar))
        {
            if (outStation != nullptr)
                *outStation = _vehicleStationIndex.ToUnderlying();
            return _vehicleMotionTrackFlags;
        }
        if (_vehicleVelocityF64E08 >= 0)
        {
            spriteId = car->next_vehicle_on_train;
        }
        else
        {
            if (car == gCurrentVehicle)
            {
                break;
            }
            spriteId = car->prev_vehicle_on_ride;
        }
    }
    // Loc6DC144
    vehicle = gCurrentVehicle;

    carEntry = vehicle->Entry();
    // eax
    int32_t totalAcceleration = 0;
    // ebp
    int32_t totalMass = 0;
    // ebx
    int32_t numVehicles = 0;

    for (; vehicle != nullptr; vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        numVehicles++;
        totalMass += vehicle->mass;
        totalAcceleration += vehicle->acceleration;
    }

    vehicle = gCurrentVehicle;
    int32_t newAcceleration = (totalAcceleration / numVehicles) * 21;
    if (newAcceleration < 0)
    {
        newAcceleration += 511;
    }
    newAcceleration >>= 9;

    int32_t curAcceleration = newAcceleration;
    curAcceleration -= vehicle->velocity / 4096;
    curAcceleration -= GetAccelerationDecrease2(vehicle->velocity, totalMass);

    if (carEntry->flags.has(CarEntryFlag::isPowered))
    {
        curAcceleration = vehicle->UpdateTrackMotionPoweredRideAcceleration(carEntry, totalMass, curAcceleration);
    }
    else if (curAcceleration <= 0 && curAcceleration >= -500)
    {
        // Probably moving slowly on a flat track piece, low rolling resistance and drag.

        if (vehicle->velocity <= 0.5_mph && vehicle->velocity >= 0)
        {
            // Vehicle is creeping forwards very slowly (less than ~2km/h), boost speed a bit.
            curAcceleration += 400;
        }
    }

    if (vehicle->GetTrackType() == TrackElemType::watersplash)
    {
        if (vehicle->track_progress >= 48 && vehicle->track_progress <= 128)
        {
            curAcceleration -= vehicle->velocity >> 6;
        }
    }

    if (rideEntry->flags.has(RideEntryFlag::playSplashSoundSlide))
    {
        if (vehicle->IsHead())
        {
            if (vehicle->IsOnCoveredTrack())
            {
                if (vehicle->velocity > 2.0_mph)
                {
                    curAcceleration -= vehicle->velocity >> 6;
                }
            }
        }
    }

    vehicle->acceleration = curAcceleration;

    // hook_setreturnregisters(&regs);
    if (outStation != nullptr)
        *outStation = _vehicleStationIndex.ToUnderlying();
    return _vehicleMotionTrackFlags;
}
