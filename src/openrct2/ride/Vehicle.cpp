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
#include "../actions/ride/RideSetStatusAction.h"
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
constexpr int16_t kVehicleStoppingSpinSpeed = 600;

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

    flags.set(VehicleFlag::moveSingleCar, VehicleFlag::collisionDisabled);
    UpdateTrackMotion(nullptr);
    flags.unset(VehicleFlag::moveSingleCar, VehicleFlag::collisionDisabled);
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
        if (vehicle->flags.has(VehicleFlag::carIsBroken) && vehicle->restraints_position != 0
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

        if (vehicle->flags.has(VehicleFlag::carIsBroken) && vehicle->restraints_position != 0xFF
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
        flags.unset(VehicleFlag::testing);

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
        if (trackElemType == TrackElemType::poweredLift || flags.has(VehicleFlag::onLiftHill))
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

    if (flags.has(VehicleFlag::testing))
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
                flags.set(VehicleFlag::stoppedBySafetyCutout);
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
    flags.unset(VehicleFlag::testing);
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
    flags.set(VehicleFlag::testing);
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
        if (flags.has(VehicleFlag::trainIsBroken))
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
            if (flags.has(VehicleFlag::testing))
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

bool Vehicle::IsCableLift() const
{
    return ride_subtype == kObjectEntryIndexNull;
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

    bool travellingForwards = !flags.has(VehicleFlag::poweredCarInReverse);

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
        vehicle->flags.unset(VehicleFlag::collisionDisabled);
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
    stream << flags.holder;
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
