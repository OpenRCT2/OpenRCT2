/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Vehicle.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Editor.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../actions/RideSetStatusAction.h"
#include "../audio/Audio.h"
#include "../config/Config.h"
#include "../core/EnumUtils.hpp"
#include "../core/FixedPoint.hpp"
#include "../core/Memory.hpp"
#include "../core/Speed.hpp"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/EntityTweener.h"
#include "../entity/Particle.h"
#include "../entity/Yaw.hpp"
#include "../interface/Viewport.h"
#include "../localisation/Formatter.h"
#include "../management/NewsItem.h"
#include "../math/Trigonometry.hpp"
#include "../object/SmallSceneryEntry.h"
#include "../paint/vehicle/Vehicle.MiniGolf.h"
#include "../profiling/Profiling.h"
#include "../rct12/RCT12.h"
#include "../scenario/Scenario.h"
#include "../scripting/HookEngine.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"
#include "../world/tile_element/WallElement.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"
#include "TrainManager.h"
#include "VehicleData.h"
#include "VehicleGeometry.h"
#include "VehicleSubpositionData.h"

#include <cassert>
#include <iterator>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::TrackMetaData;
using namespace OpenRCT2::RideVehicle;

constexpr int16_t kVehicleMaxSpinSpeedForStopping = 700;
constexpr int16_t kVehicleMaxSpinSpeedWaterRide = 512;
constexpr int16_t kVehicleMinSpinSpeedWaterRide = -kVehicleMaxSpinSpeedWaterRide;
constexpr int16_t kVehicleStoppingSpinSpeed = 600;

constexpr uint8_t kTrackSpeedShiftAmount = 16;
constexpr uint8_t kBoosterAccelerationShiftAmount = 16;

Vehicle* gCurrentVehicle;

uint8_t _vehicleBreakdown;
StationIndex _vehicleStationIndex;
uint32_t _vehicleMotionTrackFlags;
int32_t _vehicleVelocityF64E08;
int32_t _vehicleVelocityF64E0C;
int32_t _vehicleUnkF64E10;
uint8_t _vehicleF64E2C;
Vehicle* _vehicleFrontVehicle;
CoordsXYZ _vehicleCurPosition;

/** rct2: 0x009A37C4 */
static constexpr CoordsXY kSurroundingTiles[] = {
    { 0, 0 },
    { 0, +kCoordsXYStep },
    { +kCoordsXYStep, 0 },
    { 0, -kCoordsXYStep },
    { 0, -kCoordsXYStep },
    { -kCoordsXYStep, 0 },
    { -kCoordsXYStep, 0 },
    { 0, +kCoordsXYStep },
    { 0, +kCoordsXYStep },
};

static constexpr SoundId kDoorOpenSoundIds[] = {
    SoundId::null,       // DoorSoundType::none
    SoundId::doorOpen,   // DoorSoundType::door
    SoundId::portcullis, // DoorSoundType::portcullis
};
static_assert(std::size(kDoorOpenSoundIds) == kDoorSoundTypeCount);

static constexpr SoundId kDoorCloseSoundIds[] = {
    SoundId::null,       // DoorSoundType::none
    SoundId::doorClose,  // DoorSoundType::door
    SoundId::portcullis, // DoorSoundType::portcullis
};
static_assert(std::size(kDoorCloseSoundIds) == kDoorSoundTypeCount);

template<>
bool EntityBase::Is<Vehicle>() const
{
    return Type == EntityType::vehicle;
}

static bool vehicle_move_info_valid(
    VehicleTrackSubposition trackSubposition, TrackElemType type, uint8_t direction, int32_t offset)
{
    uint16_t typeAndDirection = (EnumValue(type) << 2) | (direction & 3);

    if (trackSubposition >= VehicleTrackSubposition{ std::size(gTrackVehicleInfo) })
    {
        return false;
    }
    int32_t size = 0;
    switch (trackSubposition)
    {
        case VehicleTrackSubposition::Default:
            size = VehicleTrackSubpositionSizeDefault;
            break;
        case VehicleTrackSubposition::ChairliftGoingOut:
            size = 692;
            break;
        case VehicleTrackSubposition::ChairliftGoingBack:
        case VehicleTrackSubposition::ChairliftEndBullwheel:
        case VehicleTrackSubposition::ChairliftStartBullwheel:
            size = 404;
            break;
        case VehicleTrackSubposition::GoKartsLeftLane:
        case VehicleTrackSubposition::GoKartsRightLane:
            size = 1204;
            break;
        case VehicleTrackSubposition::GoKartsMovingToRightLane:
        case VehicleTrackSubposition::GoKartsMovingToLeftLane:
            size = 568;
            break;
        case VehicleTrackSubposition::MiniGolfPathA9: // VehicleTrackSubposition::MiniGolfStart9
        case VehicleTrackSubposition::MiniGolfBallPathA10:
        case VehicleTrackSubposition::MiniGolfPathB11:
        case VehicleTrackSubposition::MiniGolfBallPathB12:
        case VehicleTrackSubposition::MiniGolfPathC13:
        case VehicleTrackSubposition::MiniGolfBallPathC14:
            size = 824;
            break;
        case VehicleTrackSubposition::ReverserRCFrontBogie:
        case VehicleTrackSubposition::ReverserRCRearBogie:
            size = 868;
            break;
        default:
            break;
    }
    if (typeAndDirection >= size)
    {
        return false;
    }
    if (offset >= gTrackVehicleInfo[EnumValue(trackSubposition)][typeAndDirection]->size)
    {
        return false;
    }
    return true;
}

static const VehicleInfo* vehicle_get_move_info(
    VehicleTrackSubposition trackSubposition, TrackElemType type, uint8_t direction, int32_t offset)
{
    uint16_t typeAndDirection = (EnumValue(type) << 2) | (direction & 3);

    if (!vehicle_move_info_valid(trackSubposition, type, direction, offset))
    {
        static constexpr VehicleInfo zero = {};
        return &zero;
    }
    return &gTrackVehicleInfo[EnumValue(trackSubposition)][typeAndDirection]->info[offset];
}

const VehicleInfo* Vehicle::GetMoveInfo() const
{
    return vehicle_get_move_info(TrackSubposition, GetTrackType(), GetTrackDirection(), track_progress);
}

uint16_t VehicleGetMoveInfoSize(VehicleTrackSubposition trackSubposition, TrackElemType type, uint8_t direction)
{
    uint16_t typeAndDirection = (EnumValue(type) << 2) | (direction & 3);

    if (!vehicle_move_info_valid(trackSubposition, type, direction, 0))
    {
        return 0;
    }
    return gTrackVehicleInfo[EnumValue(trackSubposition)][typeAndDirection]->size;
}

uint16_t Vehicle::GetTrackProgress() const
{
    return VehicleGetMoveInfoSize(TrackSubposition, GetTrackType(), GetTrackDirection());
}

void Vehicle::ApplyMass(int16_t appliedMass)
{
    mass = std::clamp<int32_t>(mass + appliedMass, 1, std::numeric_limits<decltype(mass)>::max());
}

void Vehicle::MoveRelativeDistance(int32_t distance)
{
    remaining_distance += distance;

    SetFlag(VehicleFlags::MoveSingleCar | VehicleFlags::CollisionDisabled);
    UpdateTrackMotion(nullptr);
    ClearFlag(VehicleFlags::MoveSingleCar | VehicleFlags::CollisionDisabled);
}

void Vehicle::UpdateTrackChange()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    const auto moveInfo = GetMoveInfo();
    if (moveInfo == nullptr || moveInfo->isInvalid())
        return;

    _vehicleCurPosition = TrackLocation
        + CoordsXYZ{ moveInfo->x, moveInfo->y, moveInfo->z + GetRideTypeDescriptor((*curRide).type).Heights.VehicleZOffset };
    Orientation = moveInfo->yaw;
    roll = moveInfo->roll;
    pitch = moveInfo->pitch;
    MoveTo(_vehicleCurPosition);
}

Vehicle* TryGetVehicle(EntityId spriteIndex)
{
    return getGameState().entities.TryGetEntity<Vehicle>(spriteIndex);
}

/**
 *
 *  rct2: 0x006D4204
 */
void VehicleUpdateAll()
{
    PROFILED_FUNCTION();

    if (gLegacyScene == LegacyScene::scenarioEditor)
        return;

    if (gLegacyScene == LegacyScene::trackDesigner && getGameState().editorStep != EditorStep::RollercoasterDesigner)
        return;

    for (auto vehicle : TrainManager::View())
    {
        vehicle->Update();
    }
}

/**
 *
 *  rct2: 0x006D6956
 * @returns true when all closed
 */
bool Vehicle::CloseRestraints()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return true;

    bool restraintsClosed = true;
    for (Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(Id); vehicle != nullptr;
         vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        if (vehicle->HasFlag(VehicleFlags::CarIsBroken) && vehicle->restraints_position != 0
            && (curRide->breakdownReasonPending == BREAKDOWN_RESTRAINTS_STUCK_OPEN
                || curRide->breakdownReasonPending == BREAKDOWN_DOORS_STUCK_OPEN))
        {
            if (!(curRide->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN))
            {
                curRide->lifecycleFlags |= RIDE_LIFECYCLE_BROKEN_DOWN;

                RideBreakdownAddNewsItem(*curRide);

                curRide->windowInvalidateFlags.set(
                    RideInvalidateFlag::main, RideInvalidateFlag::list, RideInvalidateFlag::maintenance);

                curRide->mechanicStatus = MechanicStatus::calling;

                Vehicle* broken_vehicle = getGameState().entities.GetEntity<Vehicle>(curRide->vehicles[curRide->brokenTrain]);
                if (broken_vehicle != nullptr)
                {
                    curRide->inspectionStation = broken_vehicle->current_station;
                }
                curRide->breakdownReason = curRide->breakdownReasonPending;
            }
        }
        else
        {
            vehicle->restraints_position = std::max(vehicle->restraints_position - 20, 0);
            if (vehicle->restraints_position == 0)
            {
                continue;
            }
        }
        vehicle->Invalidate();
        restraintsClosed = false;
    }

    return restraintsClosed;
}

/**
 *
 *  rct2: 0x006D6A2C
 * @returns true when all open
 */
bool Vehicle::OpenRestraints()
{
    int32_t restraintsOpen = true;
    for (Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(Id); vehicle != nullptr;
         vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        vehicle->SwingPosition = 0;
        vehicle->SwingSpeed = 0;
        vehicle->SwingSprite = 0;

        auto curRide = vehicle->GetRide();
        if (curRide == nullptr)
            continue;

        auto rideEntry = vehicle->GetRideEntry();
        if (rideEntry == nullptr)
        {
            continue;
        }

        const auto& carEntry = rideEntry->Cars[vehicle->vehicle_type];

        if (carEntry.flags.has(CarEntryFlag::hasSpinning))
        {
            // If the vehicle is a spinner it must be spinning slow
            // For vehicles without additional frames there are 4 rotations it can unload from
            // For vehicles with additional frames it must be facing forward
            if (abs(vehicle->spin_speed) <= kVehicleMaxSpinSpeedForStopping && !(vehicle->spin_sprite & 0x30)
                && (!carEntry.flags.has(CarEntryFlag::hasSpinningCombinedWithNonSpinning) || !(vehicle->spin_sprite & 0xF8)))
            {
                vehicle->spin_speed = 0;
            }
            else
            {
                restraintsOpen = false;

                if (abs(vehicle->spin_speed) < kVehicleStoppingSpinSpeed)
                {
                    // Note will look odd if spinning right.
                    vehicle->spin_speed = kVehicleStoppingSpinSpeed;
                }
                int16_t value = vehicle->spin_speed / 256;
                vehicle->spin_sprite += value;
                vehicle->spin_speed -= value;

                vehicle->Invalidate();
                continue;
            }
        }
        if (carEntry.animation == CarEntryAnimation::ObservationTower && vehicle->animation_frame != 0)
        {
            if (vehicle->animationState <= 0xCCCC)
            {
                vehicle->animationState += carEntry.AnimationSpeed;
            }
            else
            {
                vehicle->animationState = 0;
                vehicle->animation_frame++;
                vehicle->animation_frame %= carEntry.AnimationFrames;
                vehicle->Invalidate();
            }
            restraintsOpen = false;
            continue;
        }
        if (carEntry.animation == CarEntryAnimation::AnimalFlying
            && (vehicle->animation_frame != 0 || vehicle->animationState > 0))
        {
            vehicle->UpdateAnimationAnimalFlying();
            restraintsOpen = false;
            continue;
        }

        if (vehicle->HasFlag(VehicleFlags::CarIsBroken) && vehicle->restraints_position != 0xFF
            && (curRide->breakdownReasonPending == BREAKDOWN_RESTRAINTS_STUCK_CLOSED
                || curRide->breakdownReasonPending == BREAKDOWN_DOORS_STUCK_CLOSED))
        {
            if (!(curRide->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN))
            {
                curRide->lifecycleFlags |= RIDE_LIFECYCLE_BROKEN_DOWN;

                RideBreakdownAddNewsItem(*curRide);

                curRide->windowInvalidateFlags.set(
                    RideInvalidateFlag::main, RideInvalidateFlag::list, RideInvalidateFlag::maintenance);

                curRide->mechanicStatus = MechanicStatus::calling;

                Vehicle* broken_vehicle = getGameState().entities.GetEntity<Vehicle>(curRide->vehicles[curRide->brokenTrain]);
                if (broken_vehicle != nullptr)
                {
                    curRide->inspectionStation = broken_vehicle->current_station;
                }
                curRide->breakdownReason = curRide->breakdownReasonPending;
            }
        }
        else
        {
            if (vehicle->restraints_position + 20 > 0xFF)
            {
                vehicle->restraints_position = 255;
                continue;
            }
            vehicle->restraints_position += 20;
        }
        vehicle->Invalidate();
        restraintsOpen = false;
    }

    return restraintsOpen;
}

void RideUpdateMeasurementsSpecialElements_Default(Ride& ride, const TrackElemType trackType)
{
    const auto& ted = GetTrackElementDescriptor(trackType);
    if (ted.flags.has(TrackElementFlag::normalToInversion))
    {
        if (ride.numInversions < Limits::kMaxInversions)
            ride.numInversions++;
    }
}

void RideUpdateMeasurementsSpecialElements_WaterCoaster(Ride& ride, const TrackElemType trackType)
{
    if (trackType >= TrackElemType::flatCovered && trackType <= TrackElemType::rightQuarterTurn3TilesCovered)
    {
        ride.specialTrackElements.set(SpecialElement::splash);
    }
}

/**
 *
 *  rct2: 0x006D6D1F
 */
void Vehicle::UpdateMeasurements()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (status == Status::travellingBoat)
    {
        curRide->lifecycleFlags |= RIDE_LIFECYCLE_TESTED;
        curRide->lifecycleFlags |= RIDE_LIFECYCLE_NO_RAW_STATS;
        curRide->lifecycleFlags &= ~RIDE_LIFECYCLE_TEST_IN_PROGRESS;
        ClearFlag(VehicleFlags::Testing);

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::ride, ride.ToUnderlying());
        return;
    }

    if (curRide->currentTestStation.IsNull())
        return;

    const auto& currentStation = curRide->getStation(curRide->currentTestStation);
    if (!currentStation.Entrance.IsNull())
    {
        uint8_t test_segment = curRide->currentTestSegment;
        StationIndex stationIndex = StationIndex::FromUnderlying(test_segment);
        auto& stationForTestSegment = curRide->getStation(stationIndex);

        curRide->averageSpeedTestTimeout++;
        if (curRide->averageSpeedTestTimeout >= 32)
            curRide->averageSpeedTestTimeout = 0;

        int32_t absVelocity = abs(velocity);
        if (absVelocity > curRide->maxSpeed)
        {
            curRide->maxSpeed = absVelocity;
        }

        if (curRide->averageSpeedTestTimeout == 0 && absVelocity > 0)
        {
            curRide->averageSpeed = AddClamp<int32_t>(curRide->averageSpeed, absVelocity);
            stationForTestSegment.SegmentTime++;
        }

        int32_t distance = abs(((velocity + acceleration) >> 10) * 42);
        if (NumLaps == 0)
        {
            stationForTestSegment.SegmentLength = AddClamp<int32_t>(stationForTestSegment.SegmentLength, distance);
        }

        if (curRide->getRideTypeDescriptor().flags.has(RtdFlag::hasGForces))
        {
            auto gForces = GetGForces();
            gForces.verticalG += curRide->previousVerticalG;
            gForces.lateralG += curRide->previousLateralG;
            gForces.verticalG /= 2;
            gForces.lateralG /= 2;

            curRide->previousVerticalG = gForces.verticalG;
            curRide->previousLateralG = gForces.lateralG;
            if (gForces.verticalG <= 0)
            {
                curRide->totalAirTime++;
            }

            if (gForces.verticalG > curRide->maxPositiveVerticalG)
                curRide->maxPositiveVerticalG = gForces.verticalG;

            if (gForces.verticalG < curRide->maxNegativeVerticalG)
                curRide->maxNegativeVerticalG = gForces.verticalG;

            gForces.lateralG = std::abs(gForces.lateralG);
            curRide->maxLateralG = std::max(curRide->maxLateralG, static_cast<fixed16_2dp>(gForces.lateralG));
        }
    }

    // If we have already evaluated this track piece skip to next section
    TileCoordsXYZ curTrackLoc{ TrackLocation };
    if (curTrackLoc != curRide->curTestTrackLocation)
    {
        curRide->curTestTrackLocation = curTrackLoc;

        if (currentStation.Entrance.IsNull())
            return;

        auto trackElemType = GetTrackType();
        if (trackElemType == TrackElemType::poweredLift || HasFlag(VehicleFlags::OnLiftHill))
        {
            if (!curRide->testingFlags.has(RideTestingFlag::poweredLift))
            {
                curRide->testingFlags.set(RideTestingFlag::poweredLift);
                if (curRide->numPoweredLifts < Limits::kRideMaxNumPoweredLiftsCount)
                    curRide->numPoweredLifts++;
            }
        }
        else
        {
            curRide->testingFlags.unset(RideTestingFlag::poweredLift);
        }

        const auto& rtd = curRide->getRideTypeDescriptor();
        rtd.UpdateMeasurementsSpecialElements(*curRide, trackElemType);

        switch (trackElemType)
        {
            case TrackElemType::rapids:
                curRide->specialTrackElements.set(SpecialElement::rapids);
                break;
            case TrackElemType::spinningTunnel:
                curRide->specialTrackElements.set(SpecialElement::spinningTunnel);
                break;
            case TrackElemType::waterfall:
                curRide->specialTrackElements.set(SpecialElement::waterfall);
                break;
            case TrackElemType::logFlumeReverser:
                curRide->specialTrackElements.set(SpecialElement::reverser);
                break;
            case TrackElemType::whirlpool:
                curRide->specialTrackElements.set(SpecialElement::whirlpool);
                break;
            case TrackElemType::watersplash:
                if (velocity >= 11.0_mph)
                {
                    curRide->specialTrackElements.set(SpecialElement::splash);
                }
                break;
            default:
                break;
        }

        const auto& ted = GetTrackElementDescriptor(trackElemType);
        auto testingFlags = curRide->testingFlags;
        if (testingFlags.has(RideTestingFlag::turnLeft) && ted.flags.has(TrackElementFlag::turnLeft))
        {
            // 0x800 as this is masked to kCurrentTurnCountMask
            curRide->turnCountDefault += 0x800;
        }
        else if (testingFlags.has(RideTestingFlag::turnRight) && ted.flags.has(TrackElementFlag::turnRight))
        {
            // 0x800 as this is masked to kCurrentTurnCountMask
            curRide->turnCountDefault += 0x800;
        }
        else if (testingFlags.has(RideTestingFlag::turnRight) || testingFlags.has(RideTestingFlag::turnLeft))
        {
            curRide->testingFlags.unset(
                RideTestingFlag::turnLeft, RideTestingFlag::turnRight, RideTestingFlag::turnBanked,
                RideTestingFlag::turnSloped);

            uint8_t turnType = 1;
            if (!testingFlags.has(RideTestingFlag::turnBanked))
            {
                turnType = 2;
                if (!testingFlags.has(RideTestingFlag::turnSloped))
                {
                    turnType = 0;
                }
            }
            switch (curRide->turnCountDefault >> 11)
            {
                case 0:
                    IncrementTurnCount1Element(*curRide, turnType);
                    break;
                case 1:
                    IncrementTurnCount2Elements(*curRide, turnType);
                    break;
                case 2:
                    IncrementTurnCount3Elements(*curRide, turnType);
                    break;
                default:
                    IncrementTurnCount4PlusElements(*curRide, turnType);
                    break;
            }
        }
        else
        {
            if (ted.flags.has(TrackElementFlag::turnLeft))
            {
                curRide->testingFlags.set(RideTestingFlag::turnLeft);
                curRide->turnCountDefault &= ~kCurrentTurnCountMask;

                if (ted.flags.has(TrackElementFlag::turnBanked))
                {
                    curRide->testingFlags.set(RideTestingFlag::turnBanked);
                }
                if (ted.flags.has(TrackElementFlag::turnSloped))
                {
                    curRide->testingFlags.set(RideTestingFlag::turnSloped);
                }
            }

            if (ted.flags.has(TrackElementFlag::turnRight))
            {
                curRide->testingFlags.set(RideTestingFlag::turnRight);
                curRide->turnCountDefault &= ~kCurrentTurnCountMask;

                if (ted.flags.has(TrackElementFlag::turnBanked))
                {
                    curRide->testingFlags.set(RideTestingFlag::turnBanked);
                }
                if (ted.flags.has(TrackElementFlag::turnSloped))
                {
                    curRide->testingFlags.set(RideTestingFlag::turnSloped);
                }
            }
        }

        if (testingFlags.has(RideTestingFlag::dropDown))
        {
            if (velocity < 0 || !ted.flags.has(TrackElementFlag::down))
            {
                curRide->testingFlags.unset(RideTestingFlag::dropDown);

                int16_t curZ = z / kCoordsZStep - curRide->startDropHeight;
                if (curZ < 0)
                {
                    curZ = abs(curZ);
                    if (curZ > curRide->highestDropHeight)
                    {
                        curRide->highestDropHeight = static_cast<uint8_t>(curZ);
                    }
                }
            }
        }
        else if (ted.flags.has(TrackElementFlag::down) && velocity >= 0)
        {
            curRide->testingFlags.unset(RideTestingFlag::dropUp);
            curRide->testingFlags.set(RideTestingFlag::dropDown);

            if (curRide->numDrops < Limits::kRideMaxDropsCount)
                curRide->numDrops++;

            curRide->startDropHeight = z / kCoordsZStep;
            testingFlags.unset(RideTestingFlag::dropUp);
        }

        if (testingFlags.has(RideTestingFlag::dropUp))
        {
            if (velocity > 0 || !ted.flags.has(TrackElementFlag::up))
            {
                curRide->testingFlags.unset(RideTestingFlag::dropUp);

                int16_t curZ = z / kCoordsZStep - curRide->startDropHeight;
                if (curZ < 0)
                {
                    curZ = abs(curZ);
                    if (curZ > curRide->highestDropHeight)
                    {
                        curRide->highestDropHeight = static_cast<uint8_t>(curZ);
                    }
                }
            }
        }
        else if (ted.flags.has(TrackElementFlag::up) && velocity <= 0)
        {
            curRide->testingFlags.unset(RideTestingFlag::dropDown);
            curRide->testingFlags.set(RideTestingFlag::dropUp);

            if (curRide->numDrops < Limits::kRideMaxDropsCount)
                curRide->numDrops++;

            curRide->startDropHeight = z / kCoordsZStep;
        }

        if (ted.flags.has(TrackElementFlag::helix))
        {
            if (curRide->numHelices < Limits::kMaxHelices)
                curRide->numHelices++;
        }
    }

    if (currentStation.Entrance.IsNull())
        return;

    if (x == kLocationNull)
    {
        curRide->testingFlags.unset(RideTestingFlag::sheltered);
        return;
    }

    auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });
    // If vehicle above ground.
    if (surfaceElement != nullptr && surfaceElement->GetBaseZ() <= z)
    {
        if (!TrackGetIsSheltered(CoordsXYZ{ x, y, z }))
        {
            curRide->testingFlags.unset(RideTestingFlag::sheltered);
            return;
        }
    }

    if (!curRide->testingFlags.has(RideTestingFlag::sheltered))
    {
        curRide->testingFlags.set(RideTestingFlag::sheltered);

        curRide->increaseNumShelteredSections();

        if (pitch != VehiclePitch::flat)
        {
            curRide->numShelteredSections |= ShelteredSectionsBits::kRotatingWhileSheltered;
        }

        if (roll != VehicleRoll::unbanked)
        {
            curRide->numShelteredSections |= ShelteredSectionsBits::kBankingWhileSheltered;
        }
    }

    int32_t distance = ((velocity + acceleration) >> 10) * 42;
    if (distance < 0)
        return;

    curRide->shelteredLength = AddClamp<int32_t>(curRide->shelteredLength, distance);
}

/**
 *
 *  rct2: 0x006D77F2
 */
void Vehicle::Update()
{
    if (IsCableLift())
    {
        CableLiftUpdate();
        return;
    }

    auto rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
        return;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->type >= RIDE_TYPE_COUNT)
        return;

    if (HasFlag(VehicleFlags::Testing))
        UpdateMeasurements();

    _vehicleBreakdown = 255;
    if (curRide->lifecycleFlags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        _vehicleBreakdown = curRide->breakdownReasonPending;
        auto carEntry = &rideEntry->Cars[vehicle_type];
        if (carEntry->flags.has(CarEntryFlag::isPowered) && curRide->breakdownReasonPending == BREAKDOWN_SAFETY_CUT_OUT)
        {
            if (!carEntry->flags.has(CarEntryFlag::isWaterRide) || (pitch == VehiclePitch::up25 && velocity <= 2.0_mph))
            {
                SetFlag(VehicleFlags::StoppedBySafetyCutOut);
            }
        }
    }

    switch (status)
    {
        case Status::movingToEndOfStation:
            UpdateMovingToEndOfStation();
            break;
        case Status::waitingForPassengers:
            UpdateWaitingForPassengers();
            break;
        case Status::waitingToDepart:
            UpdateWaitingToDepart();
            break;
        case Status::crashing:
        case Status::crashed:
            UpdateCrash();
            break;
        case Status::travellingDodgems:
            UpdateDodgemsMode();
            break;
        case Status::swinging:
            UpdateSwinging();
            break;
        case Status::simulatorOperating:
            UpdateSimulatorOperating();
            break;
        case Status::topSpinOperating:
            UpdateTopSpinOperating();
            break;
        case Status::ferrisWheelRotating:
            UpdateFerrisWheelRotating();
            break;
        case Status::spaceRingsOperating:
            UpdateSpaceRingsOperating();
            break;
        case Status::hauntedHouseOperating:
            UpdateHauntedHouseOperating();
            break;
        case Status::crookedHouseOperating:
            UpdateCrookedHouseOperating();
            break;
        case Status::rotating:
            UpdateRotating();
            break;
        case Status::departing:
            UpdateDeparting();
            break;
        case Status::travelling:
            UpdateTravelling();
            break;
        case Status::travellingCableLift:
            UpdateTravellingCableLift();
            break;
        case Status::travellingBoat:
            UpdateTravellingBoat();
            break;
        case Status::arriving:
            UpdateArriving();
            break;
        case Status::unloadingPassengers:
            UpdateUnloadingPassengers();
            break;
        case Status::waitingForCableLift:
            UpdateWaitingForCableLift();
            break;
        case Status::showingFilm:
            UpdateShowingFilm();
            break;
        case Status::doingCircusShow:
            UpdateDoingCircusShow();
            break;
        default:
            break;
    }

    UpdateSound();
}

std::optional<uint32_t> ride_get_train_index_from_vehicle(const Ride& ride, EntityId spriteIndex)
{
    uint32_t trainIndex = 0;
    while (ride.vehicles[trainIndex] != spriteIndex)
    {
        trainIndex++;
        if (trainIndex >= ride.numTrains)
        {
            // This should really return nullopt, but doing so
            // would break some hacked parks that hide track by setting tracked rides'
            // track type to, e.g., Crooked House
            break;
        }
        if (trainIndex >= std::size(ride.vehicles))
        {
            return std::nullopt;
        }
    }
    return { trainIndex };
}

/**
 *
 *  rct2: 0x006D91BF
 */
void Vehicle::UpdateDodgemsMode()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    const auto* rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }
    const auto& carEntry = rideEntry->Cars[vehicle_type];

    // Mark the dodgem as in use.
    if (carEntry.flags.has(CarEntryFlag::hasDodgemInUseLights) && animation_frame != 1)
    {
        animation_frame = 1;
        Invalidate();
    }

    UpdateMotionDodgems();

    // Update the length of time vehicle has been in dodgems mode
    if (sub_state++ == 0xFF)
    {
        TimeActive++;
    }

    if (curRide->lifecycleFlags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
        return;

    // Mark the dodgem as not in use.
    animation_frame = 0;
    Invalidate();
    velocity = 0;
    acceleration = 0;
    SetState(Status::unloadingPassengers);
}

/**
 *
 *  rct2: 0x006D9EB0
 */
void Vehicle::PeepEasterEggHereWeAre() const
{
    for (Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(Id); vehicle != nullptr;
         vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        for (int32_t i = 0; i < vehicle->num_peeps; ++i)
        {
            auto* curPeep = getGameState().entities.GetEntity<Guest>(vehicle->peep[i]);
            if (curPeep != nullptr && curPeep->PeepFlags & PEEP_FLAGS_HERE_WE_ARE)
            {
                curPeep->InsertNewThought(PeepThoughtType::HereWeAre, curPeep->CurrentRide);
            }
        }
    }
}

/**
 * Performed when vehicle has completed a full circuit
 *  rct2: 0x006D7338
 */
static void test_finish(Ride& ride)
{
    ride.lifecycleFlags &= ~RIDE_LIFECYCLE_TEST_IN_PROGRESS;
    ride.lifecycleFlags |= RIDE_LIFECYCLE_TESTED;
    ride.windowInvalidateFlags.set(RideInvalidateFlag::ratings);

    auto rideStations = ride.getStations();
    for (int32_t i = ride.numStations - 1; i >= 1; i--)
    {
        if (rideStations[i - 1].SegmentTime != 0)
            continue;

        uint16_t oldTime = rideStations[i - 1].SegmentTime;
        rideStations[i - 1].SegmentTime = rideStations[i].SegmentTime;
        rideStations[i].SegmentTime = oldTime;

        int32_t oldLength = rideStations[i - 1].SegmentLength;
        rideStations[i - 1].SegmentLength = rideStations[i].SegmentLength;
        rideStations[i].SegmentLength = oldLength;
    }

    uint32_t totalTime = 0;
    for (uint8_t i = 0; i < ride.numStations; ++i)
    {
        totalTime += rideStations[i].SegmentTime;
    }

    totalTime = std::max(totalTime, 1u);
    ride.averageSpeed = ride.averageSpeed / totalTime;

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::ride, ride.id.ToUnderlying());
}

void Vehicle::UpdateTestFinish()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;
    test_finish(*curRide);
    ClearFlag(VehicleFlags::Testing);
}

/**
 *
 *  rct2: 0x006D6BE7
 */
static void test_reset(Ride& ride, StationIndex curStation)
{
    ride.lifecycleFlags |= RIDE_LIFECYCLE_TEST_IN_PROGRESS;
    ride.lifecycleFlags &= ~RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.maxSpeed = 0;
    ride.averageSpeed = 0;
    ride.currentTestSegment = 0;
    ride.averageSpeedTestTimeout = 0;
    ride.maxPositiveVerticalG = MakeFixed16_2dp(1, 0);
    ride.maxNegativeVerticalG = MakeFixed16_2dp(1, 0);
    ride.maxLateralG = 0;
    ride.previousVerticalG = 0;
    ride.previousLateralG = 0;
    ride.testingFlags.clearAll();
    ride.curTestTrackLocation.SetNull();
    ride.turnCountDefault = 0;
    ride.turnCountBanked = 0;
    ride.turnCountSloped = 0;
    ride.numInversions = 0;
    ride.numHoles = 0;
    ride.shelteredEighths = 0;
    ride.numDrops = 0;
    ride.numPoweredLifts = 0;
    ride.shelteredLength = 0;
    ride.var11C = 0;
    ride.numShelteredSections = 0;
    ride.highestDropHeight = 0;
    ride.numHelices = 0;
    ride.specialTrackElements.clearAll();
    for (auto& station : ride.getStations())
    {
        station.SegmentLength = 0;
        station.SegmentTime = 0;
    }
    ride.totalAirTime = 0;
    ride.currentTestStation = curStation;

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::ride, ride.id.ToUnderlying());
}

void Vehicle::TestReset()
{
    SetFlag(VehicleFlags::Testing);
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;
    test_reset(*curRide, current_station);
}

// The result of this function is used to decide whether a vehicle on a tower ride should go further up or not.
// Therefore, it will return true if anything is amiss.
bool Vehicle::CurrentTowerElementIsTop()
{
    TileElement* tileElement = MapGetTrackElementAtOfType(TrackLocation, GetTrackType());
    if (tileElement == nullptr)
        return true;

    while (!tileElement->IsLastForTile())
    {
        tileElement++;

        if (tileElement->IsGhost())
            continue;

        if (tileElement->GetType() != TileElementType::Track)
            continue;

        const auto* trackElement = tileElement->AsTrack();
        if (trackElement->GetRideIndex() != ride)
            continue;

        if (trackElement->GetTrackType() != TrackElemType::towerSection)
            continue;

        return false;
    }

    return true;
}

/**
 *
 *  rct2: 0x006D9CE9
 */
void Vehicle::UpdateWaitingForCableLift()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    Vehicle* cableLift = getGameState().entities.GetEntity<Vehicle>(curRide->cableLift);
    if (cableLift == nullptr)
        return;

    if (cableLift->status != Status::waitingForPassengers)
        return;

    cableLift->SetState(Status::waitingToDepart, sub_state);
    cableLift->cable_lift_target = Id;
}

/**
 *
 *  rct2: 0x006D9D21
 */
void Vehicle::UpdateTravellingCableLift()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (sub_state == 0)
    {
        if (HasFlag(VehicleFlags::TrainIsBroken))
        {
            if (curRide->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN)
                return;

            curRide->lifecycleFlags |= RIDE_LIFECYCLE_BROKEN_DOWN;
            RideBreakdownAddNewsItem(*curRide);
            curRide->windowInvalidateFlags.set(
                RideInvalidateFlag::main, RideInvalidateFlag::list, RideInvalidateFlag::maintenance);

            curRide->mechanicStatus = MechanicStatus::calling;
            curRide->inspectionStation = current_station;
            curRide->breakdownReason = curRide->breakdownReasonPending;
            velocity = 0;
            return;
        }

        sub_state = 1;
        PeepEasterEggHereWeAre();
        if (!(curRide->lifecycleFlags & RIDE_LIFECYCLE_TESTED))
        {
            if (HasFlag(VehicleFlags::Testing))
            {
                if (curRide->currentTestSegment + 1 < curRide->numStations)
                {
                    curRide->currentTestSegment++;
                    curRide->currentTestStation = current_station;
                }
                else
                {
                    UpdateTestFinish();
                }
            }
            else if (!(curRide->lifecycleFlags & RIDE_LIFECYCLE_TEST_IN_PROGRESS) && !IsGhost())
            {
                TestReset();
            }
        }
    }

    if (velocity <= 439800)
    {
        acceleration = 4398;
    }
    int32_t curFlags = UpdateTrackMotion(nullptr);

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_11)
    {
        SetState(Status::travelling, 1);
        lost_time_out = 0;
        return;
    }

    if (sub_state == 2)
        return;

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_3 && current_station == _vehicleStationIndex)
        return;

    sub_state = 2;

    if (curRide->isBlockSectioned())
        return;

    // This is slightly different to the vanilla function
    auto& currentStation = curRide->getStation(current_station);
    currentStation.Depart &= kStationDepartFlag;
    uint8_t waitingTime = 3;
    if (curRide->departFlags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH)
    {
        waitingTime = std::max(curRide->minWaitingTime, static_cast<uint8_t>(3));
        waitingTime = std::min(waitingTime, static_cast<uint8_t>(127));
    }

    currentStation.Depart |= waitingTime;
}

/**
 *
 *  rct2: 0x006D73D0
 * ax: verticalG
 * dx: lateralG
 * esi: vehicle
 */
GForces Vehicle::GetGForces() const
{
    int32_t gForceVert = ((static_cast<int64_t>(0x280000)) * Geometry::getPitchVector32(pitch).x) >> 32;
    gForceVert = ((static_cast<int64_t>(gForceVert)) * Geometry::getRollHorizontalComponent(roll)) >> 32;

    const auto& ted = GetTrackElementDescriptor(GetTrackType());
    const int32_t vertFactor = ted.verticalFactor(track_progress);
    const int32_t lateralFactor = ted.lateralFactor(track_progress);

    int32_t gForceLateral = 0;

    if (vertFactor != 0)
    {
        gForceVert += abs(velocity) * 98 / vertFactor;
    }

    if (lateralFactor != 0)
    {
        gForceLateral += abs(velocity) * 98 / lateralFactor;
    }

    gForceVert *= 10;
    gForceLateral *= 10;
    gForceVert >>= 16;
    gForceLateral >>= 16;
    return { static_cast<int16_t>(gForceVert & 0xFFFF), static_cast<int16_t>(gForceLateral & 0xFFFF) };
}

void Vehicle::SetMapToolbar() const
{
    auto curRide = GetRide();
    if (curRide != nullptr && curRide->type < RIDE_TYPE_COUNT)
    {
        const Vehicle* vehicle = GetHead();
        if (vehicle == nullptr)
            return;

        size_t vehicleIndex;
        for (vehicleIndex = 0; vehicleIndex < std::size(curRide->vehicles); vehicleIndex++)
            if (curRide->vehicles[vehicleIndex] == vehicle->Id)
                break;

        auto ft = Formatter();
        ft.Add<StringId>(STR_RIDE_MAP_TIP);
        ft.Add<StringId>(STR_MAP_TOOLTIP_STRINGID_STRINGID);
        curRide->formatNameTo(ft);
        ft.Add<StringId>(GetRideComponentName(GetRideTypeDescriptor(curRide->type).NameConvention.vehicle).capitalised);
        ft.Add<uint16_t>(vehicleIndex + 1);
        curRide->formatStatusTo(ft);
        auto intent = Intent(INTENT_ACTION_SET_MAP_TOOLTIP);
        intent.PutExtra(INTENT_EXTRA_FORMATTER, &ft);
        ContextBroadcastIntent(&intent);
    }
}

Vehicle* Vehicle::TrainHead() const
{
    const Vehicle* vehicle = this;
    Vehicle* prevVehicle;

    for (;;)
    {
        prevVehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
        if (prevVehicle == nullptr)
            return nullptr;
        if (prevVehicle->next_vehicle_on_train.IsNull())
            break;

        vehicle = prevVehicle;
    }

    return const_cast<Vehicle*>(vehicle);
}

Vehicle* Vehicle::TrainTail() const
{
    const Vehicle* vehicle = this;

    EntityId spriteIndex = vehicle->next_vehicle_on_train;
    while (!spriteIndex.IsNull())
    {
        vehicle = getGameState().entities.GetEntity<Vehicle>(spriteIndex);
        if (vehicle == nullptr)
        {
            return const_cast<Vehicle*>(this);
        }
        spriteIndex = vehicle->next_vehicle_on_train;
    }

    return const_cast<Vehicle*>(vehicle);
}

int32_t Vehicle::IsUsedInPairs() const
{
    return num_seats & kVehicleSeatPairFlag;
}

/**
 *
 *  rct2: 0x006DA44E
 */
int32_t Vehicle::UpdateMotionDodgems()
{
    _vehicleMotionTrackFlags = 0;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return _vehicleMotionTrackFlags;

    int32_t nextVelocity = velocity + acceleration;
    if (curRide->lifecycleFlags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)
        && curRide->breakdownReasonPending == BREAKDOWN_SAFETY_CUT_OUT)
    {
        nextVelocity = 0;
    }
    velocity = nextVelocity;

    _vehicleVelocityF64E08 = nextVelocity;
    _vehicleVelocityF64E0C = (nextVelocity / 1024) * 42;
    _vehicleUnkF64E10 = 1;

    acceleration = 0;
    if (!(curRide->lifecycleFlags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN))
        || curRide->breakdownReasonPending != BREAKDOWN_SAFETY_CUT_OUT)
    {
        if ((getGameState().currentTicks & 1) && var_34 != 0)
        {
            if (var_34 > 0)
            {
                var_34--;
                Orientation += 2;
            }
            else
            {
                var_34++;
                Orientation -= 2;
            }
            Orientation &= 0x1E;
            Invalidate();
        }
        else if ((ScenarioRand() & 0xFFFF) <= 2849)
        {
            if (var_35 & (1 << 6))
                Orientation -= 2;
            else
                Orientation += 2;
            Orientation &= 0x1E;
            Invalidate();
        }
    }

    std::optional<EntityId> collideSprite;
    if (DodgemsCollisionDirection != 0)
    {
        uint8_t oldCollisionDirection = DodgemsCollisionDirection & 0x1E;
        DodgemsCollisionDirection = 0;

        CoordsXYZ location = { x, y, z };

        location.x += Geometry::getFreeroamVehicleMovementData(oldCollisionDirection).x;
        location.y += Geometry::getFreeroamVehicleMovementData(oldCollisionDirection).y;
        location.x += Geometry::getFreeroamVehicleMovementData(oldCollisionDirection + 1).x;
        location.y += Geometry::getFreeroamVehicleMovementData(oldCollisionDirection + 1).y;

        if (collideSprite = DodgemsCarWouldCollideAt(location); !collideSprite.has_value())
        {
            MoveTo(location);
        }
    }

    remaining_distance += _vehicleVelocityF64E0C;

    if (remaining_distance >= 13962)
    {
        sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
        _vehicleCurPosition.x = x;
        _vehicleCurPosition.y = y;
        _vehicleCurPosition.z = z;

        while (true)
        {
            var_35++;
            uint8_t direction = Orientation;
            direction |= var_35 & 1;

            CoordsXY location = _vehicleCurPosition;
            location.x += Geometry::getFreeroamVehicleMovementData(direction).x;
            location.y += Geometry::getFreeroamVehicleMovementData(direction).y;

            if (collideSprite = DodgemsCarWouldCollideAt(location); collideSprite.has_value())
            {
                break;
            }

            remaining_distance -= Geometry::getFreeroamVehicleMovementData(direction).distance;
            _vehicleCurPosition.x = location.x;
            _vehicleCurPosition.y = location.y;
            if (remaining_distance < 13962)
            {
                break;
            }
            _vehicleUnkF64E10++;
        }

        if (remaining_distance >= 13962)
        {
            int32_t oldVelocity = velocity;
            remaining_distance = 0;
            velocity = 0;
            uint8_t direction = Orientation | 1;

            Vehicle* collideVehicle = getGameState().entities.GetEntity<Vehicle>(collideSprite.value());
            if (collideVehicle != nullptr)
            {
                var_34 = (ScenarioRand() & 1) ? 1 : -1;

                if (oldVelocity >= 2.0_mph)
                {
                    collideVehicle->DodgemsCollisionDirection = direction;
                    DodgemsCollisionDirection = direction ^ (1 << 4);
                }
            }
            else
            {
                var_34 = (ScenarioRand() & 1) ? 6 : -6;

                if (oldVelocity >= 2.0_mph)
                {
                    DodgemsCollisionDirection = direction ^ (1 << 4);
                }
            }
        }

        MoveTo(_vehicleCurPosition);
    }

    int32_t eax = velocity / 2;
    int32_t edx = velocity >> 8;
    edx *= edx;
    if (velocity < 0)
        edx = -edx;
    edx >>= 5;
    eax += edx;
    if (mass != 0)
    {
        eax /= mass;
    }
    const auto* rideEntry = GetRideEntry();
    const auto& carEntry = rideEntry->Cars[vehicle_type];

    if (!carEntry.flags.has(CarEntryFlag::isPowered))
    {
        acceleration = -eax;
        return _vehicleMotionTrackFlags;
    }

    int32_t momentum = (speed * mass) >> 2;
    int32_t _eax = speed << 14;
    if (HasFlag(VehicleFlags::PoweredCarInReverse))
    {
        _eax = -_eax;
    }
    _eax -= velocity;
    _eax *= powered_acceleration * 2;
    if (momentum != 0)
        _eax /= momentum;

    acceleration = _eax - eax;
    return _vehicleMotionTrackFlags;
}

/**
 *
 *  rct2: 0x006DD365
 */
static bool wouldCollideWithDodgemsTrackEdge(
    const CoordsXY& coords, const CoordsXY& trackLocation, TrackElemType trackType, uint16_t dodgemsCarRadius)
{
    int16_t rideLeft = trackLocation.x + GetDodgemsTrackSize(trackType).left;
    int16_t rideRight = trackLocation.x + GetDodgemsTrackSize(trackType).right;
    int16_t rideTop = trackLocation.y + GetDodgemsTrackSize(trackType).top;
    int16_t rideBottom = trackLocation.y + GetDodgemsTrackSize(trackType).bottom;

    return coords.x - dodgemsCarRadius < rideLeft || coords.y - dodgemsCarRadius < rideTop
        || coords.x + dodgemsCarRadius > rideRight || coords.y + dodgemsCarRadius > rideBottom;
}

std::optional<EntityId> Vehicle::DodgemsCarWouldCollideAt(const CoordsXY& coords) const
{
    auto trackType = GetTrackType();

    if (wouldCollideWithDodgemsTrackEdge(coords, TrackLocation, trackType, (var_44 * 30) >> 9))
    {
        return EntityId::GetNull();
    }

    auto location = coords;

    RideId rideIndex = ride;
    for (auto xy_offset : kSurroundingTiles)
    {
        location += xy_offset;

        for (auto vehicle2 : EntityTileList<Vehicle>(location))
        {
            if (vehicle2 == this)
                continue;
            if (vehicle2->ride != rideIndex)
                continue;

            int32_t distX = abs(coords.x - vehicle2->x);
            if (distX > 32768)
                continue;

            int32_t distY = abs(coords.y - vehicle2->y);
            if (distY > 32768)
                continue;

            int32_t ecx = (var_44 + vehicle2->var_44) / 2;
            ecx *= 30;
            ecx >>= 8;
            if (std::max(distX, distY) < ecx)
            {
                return vehicle2->Id;
            }
        }
    }

    return std::nullopt;
}

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
    if (HasFlag(VehicleFlags::StoppedBySafetyCutOut))
    {
        nextVelocity = 0;
    }
    if (HasFlag(VehicleFlags::StoppedOnHoldingBrake))
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
 *  rct2: 0x006DEDB1
 */
static void play_scenery_door_open_sound(const CoordsXYZ& loc, WallElement* tileElement)
{
    auto* wallEntry = tileElement->GetEntry();
    if (wallEntry == nullptr)
        return;

    auto doorSoundType = wallEntry->getDoorSoundType();
    if (doorSoundType == DoorSoundType::none)
        return;

    auto soundId = kDoorOpenSoundIds[EnumValue(doorSoundType)];
    Play3D(soundId, loc);
}

/**
 *
 *  rct2: 0x006DED7A
 */
static void play_scenery_door_close_sound(const CoordsXYZ& loc, WallElement* tileElement)
{
    auto* wallEntry = tileElement->GetEntry();
    if (wallEntry == nullptr)
        return;

    auto doorSoundType = wallEntry->getDoorSoundType();
    if (doorSoundType == DoorSoundType::none)
        return;

    auto soundId = kDoorCloseSoundIds[EnumValue(doorSoundType)];
    Play3D(soundId, loc);
}

template<bool isBackwards>
static void AnimateSceneryDoor(const CoordsXYZD& doorLocation, const CoordsXYZ& trackLocation, bool isLastVehicle)
{
    auto door = MapGetWallElementAt(doorLocation);
    if (door == nullptr)
    {
        return;
    }

    if (!isLastVehicle && (door->GetAnimationFrame() == 0))
    {
        door->SetAnimationIsBackwards(isBackwards);
        door->SetAnimationFrame(1);
        door->SetIsAnimating(true);
        play_scenery_door_open_sound(trackLocation, door);

        MapAnimations::MarkTileForUpdate(TileCoordsXY(doorLocation));
    }

    if (isLastVehicle)
    {
        door->SetAnimationIsBackwards(isBackwards);
        door->SetAnimationFrame(6);
        door->SetIsAnimating(true);
        play_scenery_door_close_sound(trackLocation, door);

        MapAnimations::MarkTileForUpdate(TileCoordsXY(doorLocation));
    }
}

/**
 *
 *  rct2: 0x006DEE93
 */
void Vehicle::UpdateSceneryDoor() const
{
    auto trackType = GetTrackType();
    const auto& ted = GetTrackElementDescriptor(trackType);
    const auto& trackBlock = ted.sequences[ted.numSequences - 1].clearance;
    const TrackCoordinates* trackCoordinates = &ted.coordinates;
    auto wallCoords = CoordsXYZ{ x, y, TrackLocation.z - trackBlock.z + trackCoordinates->zEnd }.ToTileStart();
    int32_t direction = (GetTrackDirection() + trackCoordinates->rotationEnd) & 3;

    AnimateSceneryDoor<false>({ wallCoords, static_cast<Direction>(direction) }, TrackLocation, next_vehicle_on_train.IsNull());
}

template<bool isBackwards>
static void AnimateLandscapeDoor(
    const CoordsXYZ& doorLocation, TrackElement& trackElement, const bool isLastVehicle, const DoorSoundType doorSound,
    const CoordsXYZ& soundLocation)
{
    const auto doorState = isBackwards ? trackElement.GetDoorAState() : trackElement.GetDoorBState();
    if (!isLastVehicle && doorState == kLandEdgeDoorFrameClosed)
    {
        if (isBackwards)
            trackElement.SetDoorAState(kLandEdgeDoorFrameOpening);
        else
            trackElement.SetDoorBState(kLandEdgeDoorFrameOpening);

        MapAnimations::CreateTemporary(doorLocation, MapAnimations::TemporaryType::landEdgeDoor);
        Play3D(kDoorOpenSoundIds[EnumValue(doorSound)], soundLocation);
    }

    if (isLastVehicle)
    {
        if (isBackwards)
            trackElement.SetDoorAState(kLandEdgeDoorFrameClosing);
        else
            trackElement.SetDoorBState(kLandEdgeDoorFrameClosing);

        MapAnimations::CreateTemporary(doorLocation, MapAnimations::TemporaryType::landEdgeDoor);
        Play3D(kDoorCloseSoundIds[EnumValue(doorSound)], soundLocation);
    }
}

static const SurfaceElement* GetSurfaceElementAfterElement(const TileElement* tileElement)
{
    while (tileElement->GetType() != TileElementType::Surface)
    {
        if (tileElement->IsLastForTile())
            return nullptr;
        tileElement++;
    }
    return tileElement->AsSurface();
}

void Vehicle::UpdateLandscapeDoors(const int32_t previousTrackHeight) const
{
    const auto* currentRide = GetRide();
    if (currentRide == nullptr || !currentRide->getRideTypeDescriptor().flags.has(RtdFlag::hasLandscapeDoors))
    {
        return;
    }

    const CoordsXYZ previousTrackLocation = CoordsXYZ(x, y, previousTrackHeight).ToTileStart();
    auto* const previousTrackElement = MapGetTrackElementAtBeforeSurfaceFromRide(previousTrackLocation, ride);
    auto* const currentTrackElement = MapGetTrackElementAtBeforeSurfaceFromRide(TrackLocation, ride);
    if (previousTrackElement != nullptr && currentTrackElement == nullptr)
    {
        const auto* const surfaceElement = GetSurfaceElementAfterElement(previousTrackElement);
        if (surfaceElement != nullptr && surfaceElement->GetBaseZ() > previousTrackLocation.z)
        {
            const auto* const edgeObject = surfaceElement->GetEdgeObject();
            if (edgeObject != nullptr && edgeObject->HasDoors)
            {
                AnimateLandscapeDoor<false>(
                    previousTrackLocation, *previousTrackElement->AsTrack(), next_vehicle_on_train.IsNull(),
                    edgeObject->doorSound, TrackLocation);
            }
        }
    }
    else if (previousTrackElement == nullptr && currentTrackElement != nullptr)
    {
        const auto* const surfaceElement = GetSurfaceElementAfterElement(currentTrackElement);
        if (surfaceElement != nullptr && surfaceElement->GetBaseZ() > TrackLocation.z)
        {
            const auto* const edgeObject = surfaceElement->GetEdgeObject();
            if (edgeObject != nullptr && edgeObject->HasDoors)
            {
                AnimateLandscapeDoor<true>(
                    TrackLocation, *currentTrackElement->AsTrack(), next_vehicle_on_train.IsNull(), edgeObject->doorSound,
                    previousTrackLocation);
            }
        }
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
    if (HasFlag(VehicleFlags::CurrentlyColliding))
    {
        ClearFlag(VehicleFlags::CurrentlyColliding);
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

/**
 *
 *  rct2: 0x006DEDE8
 */
void Vehicle::UpdateSceneryDoorBackwards() const
{
    auto trackType = GetTrackType();
    const auto& ted = GetTrackElementDescriptor(trackType);
    const TrackCoordinates* trackCoordinates = &ted.coordinates;
    auto wallCoords = CoordsXYZ{ TrackLocation, TrackLocation.z - ted.sequences[0].clearance.z + trackCoordinates->zBegin };
    int32_t direction = (GetTrackDirection() + trackCoordinates->rotationBegin) & 3;
    direction = DirectionReverse(direction);

    AnimateSceneryDoor<true>({ wallCoords, static_cast<Direction>(direction) }, TrackLocation, next_vehicle_on_train.IsNull());
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
 *  rct2: 0x006DB807
 */
void Vehicle::UpdateReverserCarBogies()
{
    const auto moveInfo = GetMoveInfo();
    MoveTo({ TrackLocation.x + moveInfo->x, TrackLocation.y + moveInfo->y, z });
}

bool Vehicle::IsCableLift() const
{
    return ride_subtype == kObjectEntryIndexNull;
}

/**
 * Collision Detection
 *  rct2: 0x006DD078
 * @param vehicle (esi)
 * @param x (ax)
 * @param y (cx)
 * @param z (dx)
 * @param otherVehicleIndex (bp)
 */
bool Vehicle::UpdateMotionCollisionDetection(const CoordsXYZ& loc, EntityId* otherVehicleIndex)
{
    if (HasFlag(VehicleFlags::CollisionDisabled))
        return false;

    auto carEntry = Entry();
    if (carEntry == nullptr)
    {
        return false;
    }

    if (!carEntry->flags.has(CarEntryFlag::useBoatHireCollisionDetection))
    {
        CollisionDetectionTimer = 0;

        // If hacking boat hire rides you can end up here
        if (otherVehicleIndex == nullptr)
            return false;

        Vehicle* collideVehicle = getGameState().entities.GetEntity<Vehicle>(*otherVehicleIndex);
        if (collideVehicle == nullptr)
            return false;

        if (this == collideVehicle)
            return false;

        int32_t x_diff = abs(loc.x - collideVehicle->x);
        if (x_diff > 0x7FFF)
            return false;

        int32_t y_diff = abs(loc.y - collideVehicle->y);
        if (y_diff > 0x7FFF)
            return false;

        int32_t z_diff = abs(loc.z - collideVehicle->z);
        if (x_diff + y_diff + z_diff > 0xFFFF)
            return false;

        uint16_t ecx = std::min(var_44 + collideVehicle->var_44, 560);
        ecx = ((ecx >> 1) * 30) >> 8;

        if (x_diff + y_diff + z_diff >= ecx)
            return false;

        uint8_t direction = (Orientation - collideVehicle->Orientation + 7) & 0x1F;
        return direction < 0xF;
    }

    CoordsXY location = loc;

    bool mayCollide = false;
    Vehicle* collideVehicle = nullptr;
    for (auto xy_offset : kSurroundingTiles)
    {
        location += xy_offset;

        for (auto vehicle2 : EntityTileList<Vehicle>(location))
        {
            if (vehicle2 == this)
                continue;

            int32_t z_diff = abs(vehicle2->z - loc.z);

            if (z_diff > 16)
                continue;

            if (vehicle2->IsCableLift())
                continue;

            auto collideCarEntry = vehicle2->Entry();
            if (collideCarEntry == nullptr)
                continue;

            if (!collideCarEntry->flags.has(CarEntryFlag::useBoatHireCollisionDetection))
                continue;

            uint32_t x_diff = abs(vehicle2->x - loc.x);
            if (x_diff > 0x7FFF)
                continue;

            uint32_t y_diff = abs(vehicle2->y - loc.y);
            if (y_diff > 0x7FFF)
                continue;

            VehicleTrackSubposition cl = std::min(TrackSubposition, vehicle2->TrackSubposition);
            VehicleTrackSubposition ch = std::max(TrackSubposition, vehicle2->TrackSubposition);
            if (cl != ch)
            {
                if (cl == VehicleTrackSubposition::GoKartsLeftLane && ch == VehicleTrackSubposition::GoKartsRightLane)
                    continue;
            }

            uint32_t ecx = var_44 + vehicle2->var_44;
            ecx = ((ecx >> 1) * 30) >> 8;

            if (x_diff + y_diff >= ecx)
                continue;

            if (!collideCarEntry->flags.has(CarEntryFlag::isGoKart))
            {
                collideVehicle = vehicle2;
                mayCollide = true;
                break;
            }

            uint8_t direction = (Orientation - vehicle2->Orientation - 6) & 0x1F;

            if (direction < 0x14)
                continue;

            const CoordsXY directionVector = Math::Trigonometry::YawToDirectionVector[Entity::Yaw::YawTo64(Orientation)];

            const CoordsXY directionVectorToVehicle2 = { vehicle2->x - loc.x, vehicle2->y - loc.y };
            const int32_t directionVectorToVehicle2Length = (directionVectorToVehicle2.x * directionVectorToVehicle2.x)
                + (directionVectorToVehicle2.y * directionVectorToVehicle2.y);

            const int32_t dotProduct = (directionVector.x * directionVectorToVehicle2.x)
                + (directionVector.y * directionVectorToVehicle2.y);

            static constexpr int32_t threshold = []() consteval {
                const constexpr float originalThreshold = 0.35f;
                const constexpr float directionVectorLength = 256.0f;
                const constexpr float thresholdLength = originalThreshold * directionVectorLength;
                return static_cast<int32_t>(thresholdLength * thresholdLength);
            }();
            static_assert(threshold == 8028);

            if (dotProduct > 0 && dotProduct * dotProduct > threshold * directionVectorToVehicle2Length)
            {
                collideVehicle = vehicle2;
                mayCollide = true;
                break;
            }
        }
        if (mayCollide)
        {
            break;
        }
    }

    if (!mayCollide || collideVehicle == nullptr)
    {
        CollisionDetectionTimer = 0;
        return false;
    }

    if (collideVehicle->status == Status::travellingBoat && sub_state == BoatHireSubState::EnteringReturnPosition)
    {
        return false;
    }

    CollisionDetectionTimer++;
    if (CollisionDetectionTimer < 200)
    {
        SetFlag(VehicleFlags::CurrentlyColliding);
        if (otherVehicleIndex != nullptr)
            *otherVehicleIndex = collideVehicle->Id;
        return true;
    }

    if (status == Status::movingToEndOfStation)
    {
        if (Orientation == 0)
        {
            if (x <= collideVehicle->x)
            {
                return false;
            }
        }
        else if (Orientation == 8)
        {
            if (y >= collideVehicle->y)
            {
                return false;
            }
        }
        else if (Orientation == 16)
        {
            if (x >= collideVehicle->x)
            {
                return false;
            }
        }
        else if (Orientation == 24)
        {
            if (y <= collideVehicle->y)
            {
                return false;
            }
        }
    }

    if ((collideVehicle->status == Status::travellingBoat) && (status != Status::arriving) && (status != Status::travelling))
    {
        return false;
    }

    SetFlag(VehicleFlags::CurrentlyColliding);
    if (otherVehicleIndex != nullptr)
        *otherVehicleIndex = collideVehicle->Id;
    return true;
}

/**
 *
 *  rct2: 0x006DB7D6
 */
void Vehicle::ReverseReverserCar()
{
    Vehicle* previousVehicle = getGameState().entities.GetEntity<Vehicle>(prev_vehicle_on_ride);
    Vehicle* nextVehicle = getGameState().entities.GetEntity<Vehicle>(next_vehicle_on_ride);
    if (previousVehicle == nullptr || nextVehicle == nullptr)
    {
        return;
    }

    track_progress = 168;
    vehicle_type ^= 1;

    previousVehicle->track_progress = 86;
    nextVehicle->track_progress = 158;

    nextVehicle->UpdateReverserCarBogies();
    previousVehicle->UpdateReverserCarBogies();
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
        if (track_progress > 3 && !HasFlag(VehicleFlags::PoweredCarInReverse))
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

        if (PitchAndRollStart(HasFlag(VehicleFlags::CarIsInverted), tileElement) != pitchAndRollEnd)
        {
            return false;
        }

        // Update VehicleFlags::CarIsInverted flag
        const auto previousCarIsInverted = HasFlag(VehicleFlags::CarIsInverted);
        ClearFlag(VehicleFlags::CarIsInverted);
        {
            auto rideType = ::GetRide(tileElement->AsTrack()->GetRideIndex())->type;
            if (GetRideTypeDescriptor(rideType).flags.has(RtdFlag::hasInvertedVariant))
            {
                if (tileElement->AsTrack()->IsInverted())
                {
                    SetFlag(VehicleFlags::CarIsInverted);
                }
            }
            if (previousCarIsInverted != HasFlag(VehicleFlags::CarIsInverted))
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
    ClearFlag(VehicleFlags::OnLiftHill);
    if (tileElement->AsTrack()->HasChain())
    {
        SetFlag(VehicleFlags::OnLiftHill);
    }

    trackType = tileElement->AsTrack()->GetTrackType();
    if (trackType != TrackElemType::brakes)
    {
        target_seat_rotation = tileElement->AsTrack()->GetSeatRotation();
    }
    SetTrackDirection(location.direction);
    SetTrackType(trackType);
    PopulateBrakeSpeed(TrackLocation, *tileElement->AsTrack());
    if (HasFlag(VehicleFlags::StoppedOnHoldingBrake) && vertical_drop_countdown <= 0)
    {
        ClearFlag(VehicleFlags::StoppedOnHoldingBrake);
    }
    if (trackType == TrackElemType::onRidePhoto)
    {
        tileElement->AsTrack()->SetPhotoTimeout();
        MapAnimations::CreateTemporary(TrackLocation, MapAnimations::TemporaryType::onRidePhoto);
    }
    if (trackType == TrackElemType::rotationControlToggle)
    {
        Flags ^= VehicleFlags::SpinningIsLocked;
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
                if (!HasFlag(VehicleFlags::StoppedOnHoldingBrake))
                {
                    if (track_progress >= 8)
                    {
                        acceleration = -_vehicleVelocityF64E08 * 16;
                        if (track_progress >= 24)
                        {
                            SetFlag(VehicleFlags::StoppedOnHoldingBrake);
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

        if (PitchAndRollEnd(curRide, HasFlag(VehicleFlags::CarIsInverted), trackType, tileElement) != pitchAndRollStart)
        {
            return false;
        }

        // Update VehicleFlags::CarIsInverted
        const auto previousCarIsInverted = HasFlag(VehicleFlags::CarIsInverted);
        ClearFlag(VehicleFlags::CarIsInverted);
        if (GetRideTypeDescriptor(curRide.type).flags.has(RtdFlag::hasInvertedVariant))
        {
            if (tileElement->AsTrack()->IsInverted())
            {
                SetFlag(VehicleFlags::CarIsInverted);
            }
            if (previousCarIsInverted != HasFlag(VehicleFlags::CarIsInverted))
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
            SetFlag(VehicleFlags::OnLiftHill);
        }
    }
    else
    {
        if (HasFlag(VehicleFlags::OnLiftHill))
        {
            ClearFlag(VehicleFlags::OnLiftHill);
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
    if (HasFlag(VehicleFlags::StoppedOnHoldingBrake) && vertical_drop_countdown <= 0)
    {
        ClearFlag(VehicleFlags::StoppedOnHoldingBrake);
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
    if (HasFlag(VehicleFlags::PoweredCarInReverse))
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

    if (!car.HasFlag(VehicleFlags::MoveSingleCar))
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
    if (_vehicleVelocityF64E08 < 0 && !vehicle->HasFlag(VehicleFlags::MoveSingleCar))
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
        if (car->HasFlag(VehicleFlags::OnLiftHill))
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL;
        }
        if (car->HasFlag(VehicleFlags::MoveSingleCar))
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

const RideObjectEntry* Vehicle::GetRideEntry() const
{
    return GetRideEntryByIndex(ride_subtype);
}

const CarEntry* Vehicle::Entry() const
{
    const auto* rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
    {
        return nullptr;
    }
    return &rideEntry->Cars[vehicle_type];
}

Ride* Vehicle::GetRide() const
{
    return ::GetRide(ride);
}

int32_t Vehicle::NumPeepsUntilTrainTail() const
{
    int32_t numPeeps = 0;
    for (const Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(Id); vehicle != nullptr;
         vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        numPeeps += vehicle->num_peeps;
    }

    return numPeeps;
}

/**
 *
 *  rct2: 0x006DA1EC
 */
void Vehicle::InvalidateWindow()
{
    auto intent = Intent(INTENT_ACTION_INVALIDATE_VEHICLE_WINDOW);
    intent.PutExtra(INTENT_EXTRA_VEHICLE, this);
    ContextBroadcastIntent(&intent);
}

void Vehicle::UpdateCrossings() const
{
    auto curRide = GetRide();
    if (curRide == nullptr)
    {
        return;
    }

    // Parks may have rides hacked into the path.
    // Limit path blocking to rides actually supporting level crossings to prevent peeps getting stuck everywhere.
    if (!GetRideTypeDescriptor(curRide->type).flags.has(RtdFlag::supportsLevelCrossings))
    {
        return;
    }

    // In shuttle mode, only the train head is considered to be travelling backwards
    // To prevent path getting blocked incorrectly, only update crossings when this is the train head
    if (curRide->mode == RideMode::shuttle && TrainHead() != this)
    {
        return;
    }

    const Vehicle* frontVehicle{};
    const Vehicle* backVehicle{};

    bool travellingForwards = !HasFlag(VehicleFlags::PoweredCarInReverse);

    if (travellingForwards)
    {
        frontVehicle = this;
        backVehicle = TrainTail();
    }
    else
    {
        frontVehicle = TrainTail();
        backVehicle = this;
    }

    TrackBeginEnd output{};
    int32_t direction{};

    CoordsXYE xyElement = { frontVehicle->TrackLocation,
                            MapGetTrackElementAtOfTypeSeq(frontVehicle->TrackLocation, frontVehicle->GetTrackType(), 0) };
    int32_t curZ = frontVehicle->TrackLocation.z;

    if (xyElement.element != nullptr && status != Status::arriving)
    {
        int16_t autoReserveAhead = 4 + abs(velocity) / 150000;
        int16_t crossingBonus = 0;
        bool playedClaxon = false;

        // vehicle positions mean we have to take larger
        //  margins for travelling backwards
        if (!travellingForwards)
        {
            autoReserveAhead += 1;
        }

        while (true)
        {
            auto* pathElement = MapGetPathElementAt(TileCoordsXYZ(CoordsXYZ{ xyElement, xyElement.element->GetBaseZ() }));
            if (pathElement != nullptr)
            {
                if (!playedClaxon && !pathElement->IsBlockedByVehicle())
                {
                    Claxon();
                    playedClaxon = true;
                }
                crossingBonus = 4;
                pathElement->SetIsBlockedByVehicle(true);
            }
            else
            {
                crossingBonus = 0;
            }

            if (--autoReserveAhead + crossingBonus <= 0)
            {
                break;
            }

            curZ = xyElement.element->BaseHeight;

            if (travellingForwards)
            {
                if (!TrackBlockGetNext(&xyElement, &xyElement, &curZ, &direction))
                {
                    break;
                }
            }
            else
            {
                if (!TrackBlockGetPrevious(xyElement, &output))
                {
                    break;
                }
                xyElement.x = output.begin_x;
                xyElement.y = output.begin_y;
                xyElement.element = output.begin_element;
            }

            // Ensure trains near a station don't block possible crossings after the stop,
            // except when they are departing
            if (xyElement.element->AsTrack()->IsStation() && status != Status::departing)
            {
                break;
            }
        }
    }

    xyElement = { backVehicle->TrackLocation,
                  MapGetTrackElementAtOfTypeSeq(backVehicle->TrackLocation, backVehicle->GetTrackType(), 0) };
    if (xyElement.element == nullptr)
    {
        return;
    }

    // Ensure departing trains don't clear blocked crossings behind them that might already be blocked by another incoming train
    uint8_t freeCount = travellingForwards && status != Status::departing ? 3 : 1;
    while (freeCount-- > 0)
    {
        if (travellingForwards)
        {
            if (TrackBlockGetPrevious(xyElement, &output))
            {
                xyElement.x = output.begin_x;
                xyElement.y = output.begin_y;
                xyElement.element = output.begin_element;
            }
        }

        auto* pathElement = MapGetPathElementAt(TileCoordsXYZ(CoordsXYZ{ xyElement, xyElement.element->GetBaseZ() }));
        if (pathElement != nullptr)
        {
            pathElement->SetIsBlockedByVehicle(false);
        }
    }
}

Vehicle* Vehicle::GetHead()
{
    auto v = this;
    while (v != nullptr && !v->IsHead())
    {
        v = getGameState().entities.GetEntity<Vehicle>(v->prev_vehicle_on_ride);
    }
    return v;
}

const Vehicle* Vehicle::GetHead() const
{
    return (const_cast<Vehicle*>(this)->GetHead());
}

Vehicle* Vehicle::GetCar(size_t carIndex) const
{
    auto car = const_cast<Vehicle*>(this);
    for (; carIndex != 0; carIndex--)
    {
        car = getGameState().entities.GetEntity<Vehicle>(car->next_vehicle_on_train);
        if (car == nullptr)
        {
            LOG_ERROR("Tried to get non-existent car from index!");
            return nullptr;
        }
    }
    return car;
}

void Vehicle::SetState(Status vehicleStatus, uint8_t subState)
{
    status = vehicleStatus;
    sub_state = subState;
    InvalidateWindow();
}

bool Vehicle::IsGhost() const
{
    auto r = GetRide();
    return r != nullptr && r->status == RideStatus::simulating;
}

void Vehicle::EnableCollisionsForTrain()
{
    assert(this->IsHead());
    for (auto vehicle = this; vehicle != nullptr;
         vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        vehicle->ClearFlag(VehicleFlags::CollisionDisabled);
    }
}

void Vehicle::Serialise(DataSerialiser& stream)
{
    EntityBase::Serialise(stream);
    stream << SubType;
    stream << pitch;
    stream << roll;
    stream << remaining_distance;
    stream << velocity;
    stream << acceleration;
    stream << ride;
    stream << vehicle_type;
    stream << colours;
    stream << track_progress;
    stream << TrackTypeAndDirection;
    stream << TrackLocation;
    stream << next_vehicle_on_train;
    stream << prev_vehicle_on_ride;
    stream << next_vehicle_on_ride;
    stream << var_44;
    stream << mass;
    stream << Flags;
    stream << SwingSprite;
    stream << current_station;
    stream << SwingPosition;
    stream << SwingSpeed;
    stream << status;
    stream << sub_state;
    stream << peep;
    stream << peep_tshirt_colours;
    stream << num_seats;
    stream << num_peeps;
    stream << next_free_seat;
    stream << restraints_position;
    stream << spin_speed;
    stream << sound2_flags;
    stream << spin_sprite;
    stream << sound1_id;
    stream << sound1_volume;
    stream << sound2_id;
    stream << sound2_volume;
    stream << dopplerShift;
    stream << var_C0;
    stream << speed;
    stream << powered_acceleration;
    stream << DodgemsCollisionDirection;
    stream << animation_frame;
    stream << animationState;
    stream << scream_sound_id;
    stream << TrackSubposition;
    stream << NumLaps;
    stream << brake_speed;
    stream << lost_time_out;
    stream << vertical_drop_countdown;
    stream << var_D3;
    stream << mini_golf_current_animation;
    stream << mini_golf_flags;
    stream << ride_subtype;
    stream << seat_rotation;
    stream << target_seat_rotation;
    stream << BoatLocation;
    stream << BlockBrakeSpeed;
}

bool Vehicle::IsOnCoveredTrack() const
{
    return TrackElementIsCovered(GetTrackType());
}
