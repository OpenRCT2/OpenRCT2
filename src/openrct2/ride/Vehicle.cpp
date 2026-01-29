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
static bool vehicle_boat_is_location_accessible(const CoordsXYZ& location);

constexpr int16_t kVehicleMaxSpinSpeed = 1536;
constexpr int16_t kVehicleMinSpinSpeed = -kVehicleMaxSpinSpeed;
constexpr int16_t kVehicleMaxSpinSpeedForStopping = 700;
constexpr int16_t kVehicleMaxSpinSpeedWaterRide = 512;
constexpr int16_t kVehicleMinSpinSpeedWaterRide = -kVehicleMaxSpinSpeedWaterRide;
constexpr int16_t kVehicleStoppingSpinSpeed = 600;

constexpr uint8_t kTrackSpeedShiftAmount = 16;
constexpr uint8_t kBoosterAccelerationShiftAmount = 16;

Vehicle* gCurrentVehicle;

static uint8_t _vehicleBreakdown;
StationIndex _vehicleStationIndex;
uint32_t _vehicleMotionTrackFlags;
int32_t _vehicleVelocityF64E08;
int32_t _vehicleVelocityF64E0C;
int32_t _vehicleUnkF64E10;
uint8_t _vehicleF64E2C;
Vehicle* _vehicleFrontVehicle;
CoordsXYZ _vehicleCurPosition;

static constexpr SoundId _screamSetMisc[] = {
    SoundId::scream8,
    SoundId::scream1,
};
static constexpr SoundId _screamSetWooden[] = {
    SoundId::scream3, SoundId::scream1, SoundId::scream5, SoundId::scream6,
    SoundId::scream7, SoundId::scream2, SoundId::scream4,
};
static constexpr SoundId _screamSetSteel[] = {
    SoundId::scream1,
    SoundId::scream6,
};

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

#ifdef ENABLE_SCRIPTING
/**
 * Fires the "vehicle.crash" api hook
 * @param vehicleId Entity id of the vehicle that just crashed
 * @param crashId What the vehicle crashed into. Should be either "another_vehicle", "land", or "water"
 */
static void InvokeVehicleCrashHook(const EntityId vehicleId, const std::string_view crashId)
{
    auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
    if (hookEngine.HasSubscriptions(Scripting::HookType::vehicleCrash))
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        // Create event args object
        auto obj = Scripting::DukObject(ctx);
        obj.Set("id", vehicleId.ToUnderlying());
        obj.Set("crashIntoType", crashId);

        // Call the subscriptions
        auto e = obj.Take();
        hookEngine.Call(Scripting::HookType::vehicleCrash, e, true);
    }
}
#endif

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

void VehicleSoundsUpdate()
{
    auto windowManager = Ui::GetWindowManager();
    windowManager->BroadcastIntent(Intent(INTENT_ACTION_UPDATE_VEHICLE_SOUNDS));
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

void RideUpdateMeasurementsSpecialElements_MiniGolf(Ride& ride, const TrackElemType trackType)
{
    const auto& ted = GetTrackElementDescriptor(trackType);
    if (ted.flags.has(TrackElementFlag::isGolfHole))
    {
        if (ride.numHoles < Limits::kMaxGolfHoles)
            ride.numHoles++;
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

        if (curRide->getRideTypeDescriptor().HasFlag(RtdFlag::hasGForces))
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

struct SoundIdVolume
{
    SoundId id;
    uint8_t volume;
};

/**
 *
 *  rct2: 0x006D7AC0
 */
static SoundIdVolume VehicleSoundFadeInOut(
    SoundId currentSoundId, uint8_t currentVolume, SoundId targetSoundId, uint8_t targetVolume)
{
    if (currentSoundId != SoundId::null)
    {
        if (currentSoundId == targetSoundId)
        {
            currentVolume = std::min<int32_t>(currentVolume + 15, targetVolume);
            return { currentSoundId, currentVolume };
        }

        currentVolume -= 9;
        if (currentVolume >= 80)
            return { currentSoundId, currentVolume };
    }

    // Begin sound at quarter volume
    currentSoundId = targetSoundId;
    currentVolume = targetVolume == 255 ? 255 : targetVolume / 4;

    return { currentSoundId, currentVolume };
}

void Vehicle::GetLiftHillSound(const Ride& curRide, SoundIdVolume& curSound)
{
    scream_sound_id = SoundId::null;
    if (curRide.type < std::size(kRideTypeDescriptors))
    {
        // Get lift hill sound
        curSound.id = GetRideTypeDescriptor(curRide.type).LiftData.sound_id;
        curSound.volume = 243;
        if (!(sound2_flags & VEHICLE_SOUND2_FLAGS_LIFT_HILL))
            curSound.id = SoundId::null;
    }
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

/**
 *
 *  rct2: 0x006D7BCC
 */
void Vehicle::UpdateMovingToEndOfStation()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    int32_t curFlags = 0;
    int32_t station = 0;

    switch (curRide->mode)
    {
        case RideMode::upwardLaunch:
        case RideMode::rotatingLift:
        case RideMode::downwardLaunch:
        case RideMode::freefallDrop:
            if (velocity >= -131940)
            {
                acceleration = -3298;
            }
            else
            {
                velocity -= velocity / 16;
                acceleration = 0;
            }
            curFlags = UpdateTrackMotion(&station);
            if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_5))
                break;
            [[fallthrough]];
        case RideMode::dodgems:
        case RideMode::swing:
        case RideMode::rotation:
        case RideMode::forwardRotation:
        case RideMode::backwardRotation:
        case RideMode::filmAvengingAviators:
        case RideMode::filmThrillRiders:
        case RideMode::beginners:
        case RideMode::intense:
        case RideMode::berserk:
        case RideMode::mouseTails3DFilm:
        case RideMode::stormChasers3DFilm:
        case RideMode::spaceRaiders3DFilm:
        case RideMode::spaceRings:
        case RideMode::hauntedHouse:
        case RideMode::crookedHouse:
        case RideMode::circus:
            current_station = StationIndex::FromUnderlying(0);
            velocity = 0;
            acceleration = 0;
            SetState(Status::waitingForPassengers);
            break;
        default:
        {
            const auto* rideEntry = GetRideEntry();
            if (rideEntry == nullptr)
            {
                return;
            }

            const auto& carEntry = rideEntry->Cars[vehicle_type];

            if (!carEntry.flags.has(CarEntryFlag::isPowered))
            {
                if (velocity <= 131940)
                {
                    acceleration = 3298;
                }
            }
            if (velocity > 131940)
            {
                velocity -= velocity / 16;
                acceleration = 0;
            }

            curFlags = UpdateTrackMotion(&station);

            if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_1)
            {
                velocity = 0;
                acceleration = 0;
                sub_state++;

                if (curRide->mode == RideMode::race && sub_state >= 40)
                {
                    SetState(Status::waitingForPassengers);
                    break;
                }
            }
            else
            {
                if (velocity > 98955)
                {
                    sub_state = 0;
                }
            }

            if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION))
                break;

            current_station = StationIndex::FromUnderlying(station);
            velocity = 0;
            acceleration = 0;
            SetState(Status::waitingForPassengers);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006D7FB4
 */
void Vehicle::TrainReadyToDepart(uint8_t num_peeps_on_train, uint8_t num_used_seats)
{
    if (num_peeps_on_train != num_used_seats)
        return;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->status == RideStatus::open && !(curRide->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN)
        && !HasFlag(VehicleFlags::ReadyToDepart))
    {
        return;
    }

    if (!(curRide->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        const auto& rtd = curRide->getRideTypeDescriptor();
        // Original code did not check if the ride was a boat hire, causing empty boats to leave the platform when closing a
        // Boat Hire with passengers on it.
        if (curRide->status != RideStatus::closed || (curRide->numRiders != 0 && rtd.specialType != RtdSpecialType::boatHire))
        {
            curRide->getStation(current_station).TrainAtStation = RideStation::kNoTrain;
            sub_state = 2;
            return;
        }
    }

    if (curRide->mode == RideMode::forwardRotation || curRide->mode == RideMode::backwardRotation)
    {
        uint8_t seat = ((-flatRideAnimationFrame) / 8) & 0xF;
        if (!peep[seat].IsNull())
        {
            curRide->getStation(current_station).TrainAtStation = RideStation::kNoTrain;
            SetState(Status::unloadingPassengers);
            return;
        }

        if (num_peeps == 0)
            return;

        curRide->getStation(current_station).TrainAtStation = RideStation::kNoTrain;
        sub_state = 2;
        return;
    }

    if (num_peeps_on_train == 0)
        return;

    curRide->getStation(current_station).TrainAtStation = RideStation::kNoTrain;
    SetState(Status::waitingForPassengers);
}

static std::optional<uint32_t> ride_get_train_index_from_vehicle(const Ride& ride, EntityId spriteIndex)
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
 *  rct2: 0x006D7DA1
 */
void Vehicle::UpdateWaitingForPassengers()
{
    velocity = 0;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (sub_state == 0)
    {
        if (!OpenRestraints())
            return;

        auto& station = curRide->getStation(current_station);
        if (station.Entrance.IsNull())
        {
            station.TrainAtStation = RideStation::kNoTrain;
            sub_state = 2;
            return;
        }

        auto trainIndex = ride_get_train_index_from_vehicle(*curRide, Id);
        if (!trainIndex.has_value())
        {
            return;
        }

        if (station.TrainAtStation != RideStation::kNoTrain)
            return;

        station.TrainAtStation = trainIndex.value();
        sub_state = 1;
        time_waiting = 0;

        Invalidate();
        return;
    }
    if (sub_state == 1)
    {
        if (time_waiting != 0xFFFF)
            time_waiting++;

        ClearFlag(VehicleFlags::ReadyToDepart);

        // 0xF64E31, 0xF64E32, 0xF64E33
        uint8_t num_peeps_on_train = 0, num_used_seats_on_train = 0, num_seats_on_train = 0;

        for (const Vehicle* trainCar = getGameState().entities.GetEntity<Vehicle>(Id); trainCar != nullptr;
             trainCar = getGameState().entities.GetEntity<Vehicle>(trainCar->next_vehicle_on_train))
        {
            num_peeps_on_train += trainCar->num_peeps;
            num_used_seats_on_train += trainCar->next_free_seat;
            num_seats_on_train += trainCar->num_seats;
        }

        num_seats_on_train &= 0x7F;

        if (curRide->supportsStatus(RideStatus::testing))
        {
            if (time_waiting < 20)
            {
                TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                return;
            }
        }
        else
        {
            if (num_peeps_on_train == 0)
            {
                TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                return;
            }
        }

        if (curRide->getRideTypeDescriptor().HasFlag(RtdFlag::hasLoadOptions))
        {
            if (curRide->departFlags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH)
            {
                if (curRide->minWaitingTime * 32 > time_waiting)
                {
                    TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                    return;
                }
            }
            if (curRide->departFlags & RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH)
            {
                if (curRide->maxWaitingTime * 32 < time_waiting)
                {
                    SetFlag(VehicleFlags::ReadyToDepart);
                    TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                    return;
                }
            }
        }

        if (curRide->departFlags & RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES)
        {
            for (auto train_id : curRide->vehicles)
            {
                if (train_id == Id)
                    continue;

                Vehicle* train = getGameState().entities.GetEntity<Vehicle>(train_id);
                if (train == nullptr)
                    continue;

                if (train->status == Status::unloadingPassengers || train->status == Status::movingToEndOfStation)
                {
                    if (train->current_station == current_station)
                    {
                        SetFlag(VehicleFlags::ReadyToDepart);
                        TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                        return;
                    }
                }
            }
        }

        if (curRide->getRideTypeDescriptor().HasFlag(RtdFlag::hasLoadOptions)
            && curRide->departFlags & RIDE_DEPART_WAIT_FOR_LOAD)
        {
            if (num_peeps_on_train == num_seats_on_train)
            {
                SetFlag(VehicleFlags::ReadyToDepart);
                TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                return;
            }

            // any load: load=4 , full: load=3 , 3/4s: load=2 , half: load=1 , quarter: load=0
            uint8_t load = curRide->departFlags & RIDE_DEPART_WAIT_FOR_LOAD_MASK;

            // We want to wait for ceiling((load+1)/4 * num_seats_on_train) peeps, the +3 below is used instead of
            // ceil() to prevent issues on different cpus/platforms with floats. Note that vanilla RCT1/2 rounded
            // down here; our change reflects the expected behaviour for waiting for a minimum load target (see #9987)
            uint8_t peepTarget = ((load + 1) * num_seats_on_train + 3) / 4;

            if (load == 4) // take care of "any load" special case
                peepTarget = 1;

            if (num_peeps_on_train >= peepTarget)
                SetFlag(VehicleFlags::ReadyToDepart);

            TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
            return;
        }

        SetFlag(VehicleFlags::ReadyToDepart);
        TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
        return;
    }

    if (!CloseRestraints())
        return;

    velocity = 0;
    ClearFlag(VehicleFlags::WaitingOnAdjacentStation);

    if (curRide->departFlags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)
    {
        SetFlag(VehicleFlags::WaitingOnAdjacentStation);
    }

    SetState(Status::waitingToDepart);
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
 *  rct2: 0x006D80BE
 */
void Vehicle::UpdateWaitingToDepart()
{
    auto* curRide = GetRide();
    if (curRide == nullptr)
        return;

    const auto& currentStation = curRide->getStation(current_station);

    bool shouldBreak = false;
    if (curRide->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        switch (curRide->breakdownReasonPending)
        {
            case BREAKDOWN_RESTRAINTS_STUCK_CLOSED:
            case BREAKDOWN_RESTRAINTS_STUCK_OPEN:
            case BREAKDOWN_DOORS_STUCK_CLOSED:
            case BREAKDOWN_DOORS_STUCK_OPEN:
                break;
            default:
                shouldBreak = true;
                break;
        }
    }

    bool skipCheck = false;
    if (shouldBreak || curRide->status != RideStatus::open)
    {
        if (curRide->mode == RideMode::forwardRotation || curRide->mode == RideMode::backwardRotation)
        {
            uint8_t seat = ((-flatRideAnimationFrame) >> 3) & 0xF;
            if (peep[seat * 2].IsNull())
            {
                if (num_peeps == 0)
                {
                    skipCheck = true;
                }
            }
            else
            {
                if (!currentStation.Exit.IsNull())
                {
                    SetState(Status::unloadingPassengers);
                    return;
                }
            }
        }
        else
        {
            for (const Vehicle* trainCar = getGameState().entities.GetEntity<Vehicle>(Id); trainCar != nullptr;
                 trainCar = getGameState().entities.GetEntity<Vehicle>(trainCar->next_vehicle_on_train))
            {
                if (trainCar->num_peeps != 0)
                {
                    if (!currentStation.Exit.IsNull())
                    {
                        SetState(Status::unloadingPassengers);
                        return;
                    }
                    break;
                }
            }
        }
    }

    if (!skipCheck)
    {
        if (!(currentStation.Depart & kStationDepartFlag))
            return;
    }

    if (curRide->getRideTypeDescriptor().HasFlag(RtdFlag::canSynchroniseWithAdjacentStations))
    {
        if (curRide->departFlags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)
        {
            if (HasFlag(VehicleFlags::WaitingOnAdjacentStation))
            {
                if (!CanDepartSynchronised())
                {
                    return;
                }
            }
        }
    }

    SetState(Status::departing);

    if (curRide->lifecycleFlags & RIDE_LIFECYCLE_CABLE_LIFT)
    {
        CoordsXYE track;
        int32_t zUnused;
        int32_t direction;

        uint8_t trackDirection = GetTrackDirection();
        if (TrackBlockGetNextFromZero(TrackLocation, *curRide, trackDirection, &track, &zUnused, &direction, false))
        {
            if (track.element->AsTrack()->HasCableLift())
            {
                SetState(Status::waitingForCableLift, sub_state);
            }
        }
    }

    switch (curRide->mode)
    {
        case RideMode::dodgems:
            // Dodgems mode uses sub_state and TimeActive to tell how long
            // the vehicle has been ridden.
            SetState(Status::travellingDodgems);
            TimeActive = 0;
            UpdateDodgemsMode();
            break;
        case RideMode::swing:
            SetState(Status::swinging);
            NumSwings = 0;
            current_time = -1;
            UpdateSwinging();
            break;
        case RideMode::rotation:
            SetState(Status::rotating);
            NumRotations = 0;
            current_time = -1;
            UpdateRotating();
            break;
        case RideMode::filmAvengingAviators:
            SetState(Status::simulatorOperating);
            current_time = -1;
            UpdateSimulatorOperating();
            break;
        case RideMode::filmThrillRiders:
            SetState(Status::simulatorOperating, 1);
            current_time = -1;
            UpdateSimulatorOperating();
            break;
        case RideMode::beginners:
        case RideMode::intense:
        case RideMode::berserk:
            SetState(Status::topSpinOperating, sub_state);
            switch (curRide->mode)
            {
                case RideMode::beginners:
                    sub_state = 0;
                    break;
                case RideMode::intense:
                    sub_state = 1;
                    break;
                case RideMode::berserk:
                    sub_state = 2;
                    break;
                default:
                {
                    // This is workaround for multiple compilation errors of type "enumeration value ‘RIDE_MODE_*' not handled
                    // in switch [-Werror=switch]"
                }
            }
            current_time = -1;
            flatRideAnimationFrame = 0;
            flatRideSecondaryAnimationFrame = 0;
            UpdateTopSpinOperating();
            break;
        case RideMode::forwardRotation:
        case RideMode::backwardRotation:
            SetState(Status::ferrisWheelRotating, flatRideAnimationFrame);
            NumRotations = 0;
            ferris_wheel_var_0 = 8;
            ferris_wheel_var_1 = 8;
            UpdateFerrisWheelRotating();
            break;
        case RideMode::mouseTails3DFilm:
        case RideMode::stormChasers3DFilm:
        case RideMode::spaceRaiders3DFilm:
            SetState(Status::showingFilm, sub_state);
            switch (curRide->mode)
            {
                case RideMode::mouseTails3DFilm:
                    sub_state = 0;
                    break;
                case RideMode::stormChasers3DFilm:
                    sub_state = 1;
                    break;
                case RideMode::spaceRaiders3DFilm:
                    sub_state = 2;
                    break;
                default:
                {
                    // This is workaround for multiple compilation errors of type "enumeration value ‘RIDE_MODE_*' not handled
                    // in switch [-Werror=switch]"
                }
            }
            current_time = -1;
            UpdateShowingFilm();
            break;
        case RideMode::circus:
            SetState(Status::doingCircusShow);
            current_time = -1;
            UpdateDoingCircusShow();
            break;
        case RideMode::spaceRings:
            SetState(Status::spaceRingsOperating);
            flatRideAnimationFrame = 0;
            current_time = -1;
            UpdateSpaceRingsOperating();
            break;
        case RideMode::hauntedHouse:
            SetState(Status::hauntedHouseOperating);
            flatRideAnimationFrame = 0;
            current_time = -1;
            UpdateHauntedHouseOperating();
            break;
        case RideMode::crookedHouse:
            SetState(Status::crookedHouseOperating);
            flatRideAnimationFrame = 0;
            current_time = -1;
            UpdateCrookedHouseOperating();
            break;
        default:
            SetState(status);
            NumLaps = 0;
            break;
    }
}

struct SynchronisedVehicle
{
    RideId ride_id;
    StationIndex stationIndex;
    EntityId vehicle_id;
};

constexpr int32_t SYNCHRONISED_VEHICLE_COUNT = 16;

// Synchronised vehicle info
static SynchronisedVehicle _synchronisedVehicles[SYNCHRONISED_VEHICLE_COUNT] = {};

static SynchronisedVehicle* _lastSynchronisedVehicle = nullptr;

/**
 * Checks if a map position contains a synchronised ride station and adds the vehicle
 * to synchronise to the vehicle synchronisation list.
 *  rct2: 0x006DE1A4
 */
static bool try_add_synchronised_station(const CoordsXYZ& coords)
{
    // make sure we are in map bounds
    if (!MapIsLocationValid(coords))
    {
        return false;
    }

    TileElement* tileElement = GetStationPlatform({ coords, coords.z + 2 * kCoordsZStep });
    if (tileElement == nullptr)
    {
        /* No station platform element found,
         * so no station to synchronise */
        return false;
    }

    auto rideIndex = tileElement->AsTrack()->GetRideIndex();
    auto ride = GetRide(rideIndex);
    if (ride == nullptr || !(ride->departFlags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS))
    {
        /* Ride is not set to synchronise with adjacent stations. */
        return false;
    }

    /* From this point on, the ride of the map element is one that is set
     * to sync with adjacent stations, so it will return true.
     * Still to determine if a vehicle to sync can be identified. */

    auto stationIndex = tileElement->AsTrack()->GetStationIndex();

    SynchronisedVehicle* sv = _lastSynchronisedVehicle;
    sv->ride_id = rideIndex;
    sv->stationIndex = stationIndex;
    sv->vehicle_id = EntityId::GetNull();
    _lastSynchronisedVehicle++;

    /* Ride vehicles are not on the track (e.g. ride is/was under
     * construction), so just return; vehicle_id for this station
     * is SPRITE_INDEX_NULL. */
    if (!(ride->lifecycleFlags & RIDE_LIFECYCLE_ON_TRACK))
    {
        return true;
    }

    /* Station is not ready to depart, so just return;
     * vehicle_id for this station is SPRITE_INDEX_NULL. */
    if (!(ride->getStation(stationIndex).Depart & kStationDepartFlag))
    {
        return true;
    }

    // Look for a vehicle on this station waiting to depart.
    for (int32_t i = 0; i < ride->numTrains; i++)
    {
        auto* vehicle = getGameState().entities.GetEntity<Vehicle>(ride->vehicles[i]);
        if (vehicle == nullptr)
        {
            continue;
        }

        if (vehicle->status != Vehicle::Status::waitingToDepart)
        {
            continue;
        }
        if (vehicle->sub_state != 0)
        {
            continue;
        }
        if (!vehicle->HasFlag(VehicleFlags::WaitingOnAdjacentStation))
        {
            continue;
        }
        if (vehicle->current_station != stationIndex)
        {
            continue;
        }

        sv->vehicle_id = vehicle->Id;
        return true;
    }

    /* No vehicle found waiting to depart (with sync adjacent) at the
     * station, so just return; vehicle_id for this station is
     * SPRITE_INDEX_NULL. */
    return true;
}

/**
 * Checks whether a vehicle can depart a station when set to synchronise with adjacent stations.
 *  rct2: 0x006DE287
 * @param vehicle The vehicle waiting to depart.
 * @returns true if the vehicle can depart (all adjacent trains are ready or broken down), otherwise false.
 *
 * Permits vehicles to depart in two ways:
 *  Returns true, permitting the vehicle in the param to depart immediately;
 *  The vehicle flag VehicleFlags::WaitingOnAdjacentStation is cleared for those
 *  vehicles that depart in sync with the vehicle in the param.
 */
static bool ride_station_can_depart_synchronised(const Ride& ride, StationIndex stationIndex)
{
    const auto& station = ride.getStation(stationIndex);
    auto location = station.GetStart();

    auto tileElement = MapGetTrackElementAt(location);
    if (tileElement == nullptr)
    {
        return false;
    }

    // Reset the list of synchronised vehicles to empty.
    _lastSynchronisedVehicle = _synchronisedVehicles;

    /* Search for stations to sync in both directions from the current tile.
     * We allow for some space between stations, and every time a station
     *  is found we allow for space between that and the next.
     */

    int32_t direction = tileElement->GetDirectionWithOffset(1);
    constexpr uint8_t maxCheckDistance = kRideAdjacencyCheckDistance;
    uint8_t spaceBetween = maxCheckDistance;

    while (_lastSynchronisedVehicle < &_synchronisedVehicles[SYNCHRONISED_VEHICLE_COUNT - 1])
    {
        location += CoordsXYZ{ CoordsDirectionDelta[direction], 0 };
        if (try_add_synchronised_station(location))
        {
            spaceBetween = maxCheckDistance;
            continue;
        }
        if (spaceBetween-- == 0)
        {
            break;
        }
    }

    // Other search direction.
    location = station.GetStart();
    direction = DirectionReverse(direction) & 3;
    spaceBetween = maxCheckDistance;
    while (_lastSynchronisedVehicle < &_synchronisedVehicles[SYNCHRONISED_VEHICLE_COUNT - 1])
    {
        location += CoordsXYZ{ CoordsDirectionDelta[direction], 0 };
        if (try_add_synchronised_station(location))
        {
            spaceBetween = maxCheckDistance;
            continue;
        }
        if (spaceBetween-- == 0)
        {
            break;
        }
    }

    if (_lastSynchronisedVehicle == _synchronisedVehicles)
    {
        // No adjacent stations, allow depart
        return true;
    }

    for (SynchronisedVehicle* sv = _synchronisedVehicles; sv < _lastSynchronisedVehicle; sv++)
    {
        Ride* sv_ride = GetRide(sv->ride_id);

        if (!(sv_ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN))
        {
            if (sv_ride->status != RideStatus::closed)
            {
                if (sv_ride->isBlockSectioned())
                {
                    if (!(sv_ride->getStation(sv->stationIndex).Depart & kStationDepartFlag))
                    {
                        sv = _synchronisedVehicles;
                        RideId rideId = RideId::GetNull();
                        for (; sv < _lastSynchronisedVehicle; sv++)
                        {
                            if (rideId.IsNull())
                            {
                                rideId = sv->ride_id;
                            }
                            if (rideId != sv->ride_id)
                            {
                                // Here the sync-ed stations are not all from the same ride.
                                return false;
                            }
                        }

                        /* Here all the of sync-ed stations are from the same ride */
                        auto curRide = GetRide(rideId);
                        if (curRide != nullptr)
                        {
                            for (int32_t i = 0; i < curRide->numTrains; i++)
                            {
                                Vehicle* v = getGameState().entities.GetEntity<Vehicle>(curRide->vehicles[i]);
                                if (v == nullptr)
                                {
                                    continue;
                                }
                                if (v->status != Vehicle::Status::waitingToDepart && v->velocity != 0)
                                {
                                    // Here at least one vehicle on the ride is moving.
                                    return false;
                                }
                            }
                        }

                        // UNCERTAIN: is the return desired here, or rather continue on with the general checks?
                        return true;
                    }
                }
                // There is no vehicle waiting at this station to sync with.
                if (sv->vehicle_id.IsNull())
                {
                    // Check conditions for departing without all stations being in sync.
                    if (_lastSynchronisedVehicle > &_synchronisedVehicles[1])
                    {
                        // Sync condition: there are at least 3 stations to sync
                        return false;
                    }
                    RideId someRideIndex = _synchronisedVehicles[0].ride_id;
                    if (someRideIndex != ride.id)
                    {
                        // Sync condition: the first station to sync is a different ride
                        return false;
                    }

                    int32_t numTrainsAtStation = 0;
                    int32_t numTravelingTrains = 0;
                    auto currentStation = sv->stationIndex;
                    for (int32_t i = 0; i < sv_ride->numTrains; i++)
                    {
                        auto* otherVehicle = getGameState().entities.GetEntity<Vehicle>(sv_ride->vehicles[i]);
                        if (otherVehicle == nullptr)
                        {
                            continue;
                        }
                        if (otherVehicle->status != Vehicle::Status::travelling)
                        {
                            if (currentStation == otherVehicle->current_station)
                            {
                                if (otherVehicle->status == Vehicle::Status::waitingToDepart
                                    || otherVehicle->status == Vehicle::Status::movingToEndOfStation)
                                {
                                    numTrainsAtStation++;
                                }
                            }
                        }
                        else
                        {
                            numTravelingTrains++;
                        }
                    }

                    int32_t totalTrains = numTrainsAtStation + numTravelingTrains;
                    if (totalTrains != sv_ride->numTrains || numTravelingTrains >= sv_ride->numTrains / 2)
                    {
                        // if (numArrivingTrains > 0 || numTravelingTrains >= sv_ride->numTrains / 2) {
                        /* Sync condition: If there are trains arriving at the
                         * station or half or more of the ride trains are
                         * travelling, this station must be sync-ed before the
                         * trains can depart! */
                        return false;
                    }

                    /* Sync exception - train is not arriving at the station
                     * and there are less than half the trains for the ride
                     * travelling. */
                }
            }
        }
    }

    // At this point all vehicles in _snychronisedVehicles can depart.
    for (SynchronisedVehicle* sv = _synchronisedVehicles; sv < _lastSynchronisedVehicle; sv++)
    {
        auto v = getGameState().entities.GetEntity<Vehicle>(sv->vehicle_id);
        if (v != nullptr)
        {
            v->ClearFlag(VehicleFlags::WaitingOnAdjacentStation);
        }
    }

    return true;
}

bool Vehicle::CanDepartSynchronised() const
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return false;
    return ride_station_can_depart_synchronised(*curRide, current_station);
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
 *  rct2: 0x006D986C
 */
void Vehicle::UpdateTravellingBoatHireSetup()
{
    var_34 = Orientation;
    TrackLocation.x = x;
    TrackLocation.y = y;
    TrackLocation = TrackLocation.ToTileStart();

    CoordsXY location = CoordsXY(TrackLocation) + CoordsDirectionDelta[Orientation >> 3];

    BoatLocation = location;
    var_35 = 0;
    // No longer on a track so reset to 0 for import/export
    SetTrackDirection(0);
    SetTrackType(TrackElemType::flat);
    SetState(Status::travellingBoat);
    remaining_distance += 27924;

    UpdateTravellingBoat();
}

/**
 *
 *  rct2: 0x006D982F
 */
void Vehicle::UpdateDepartingBoatHire()
{
    lost_time_out = 0;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    auto& station = curRide->getStation(current_station);
    station.Depart &= kStationDepartFlag;
    uint8_t waitingTime = std::max(curRide->minWaitingTime, static_cast<uint8_t>(3));
    waitingTime = std::min(waitingTime, static_cast<uint8_t>(127));
    station.Depart |= waitingTime;
    UpdateTravellingBoatHireSetup();
}

/**
 *
 *  rct2: 0x006D845B
 */
void Vehicle::UpdateDeparting()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    const auto* rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
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

        if (rideEntry->flags & RIDE_ENTRY_FLAG_PLAY_DEPART_SOUND)
        {
            auto soundId = (rideEntry->Cars[0].soundRange == SoundRange::tramBell) ? SoundId::tram : SoundId::trainDeparting;
            Play3D(soundId, GetLocation());
        }

        if (curRide->mode == RideMode::upwardLaunch || (curRide->mode == RideMode::downwardLaunch && NumLaunches > 1))
        {
            Play3D(SoundId::rideLaunch2, GetLocation());
        }

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

    const auto& carEntry = rideEntry->Cars[vehicle_type];
    const auto& rtd = curRide->getRideTypeDescriptor();
    switch (curRide->mode)
    {
        case RideMode::reverseInclineLaunchedShuttle:
            if (velocity >= -131940)
                acceleration = -3298;
            break;
        case RideMode::poweredLaunchPasstrough:
        case RideMode::poweredLaunch:
        case RideMode::poweredLaunchBlockSectioned:
        case RideMode::limPoweredLaunch:
        case RideMode::upwardLaunch:
            if ((curRide->launchSpeed << 16) > velocity)
            {
                acceleration = curRide->launchSpeed << rtd.BoosterSettings.AccelerationFactor;
            }
            break;
        case RideMode::downwardLaunch:
            if (NumLaunches >= 1)
            {
                if ((14 << 16) > velocity)
                    acceleration = 14 << 12;
                break;
            }
            [[fallthrough]];
        case RideMode::continuousCircuit:
        case RideMode::continuousCircuitBlockSectioned:
        case RideMode::rotatingLift:
        case RideMode::freefallDrop:
        case RideMode::boatHire:
            if (carEntry.flags.has(CarEntryFlag::isPowered))
                break;

            if (velocity <= 131940)
                acceleration = 3298;
            break;
        default:
        {
            // This is workaround for multiple compilation errors of type "enumeration value ‘RIDE_MODE_*' not handled
            // in switch [-Werror=switch]"
        }
    }

    uint32_t curFlags = UpdateTrackMotion(nullptr);

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_8)
    {
        if (curRide->mode == RideMode::reverseInclineLaunchedShuttle)
        {
            velocity = -velocity;
            FinishDeparting();
            return;
        }
    }

    if (curFlags & (VEHICLE_UPDATE_MOTION_TRACK_FLAG_5 | VEHICLE_UPDATE_MOTION_TRACK_FLAG_12))
    {
        if (curRide->mode == RideMode::boatHire)
        {
            UpdateDepartingBoatHire();
            return;
        }
        if (curRide->mode == RideMode::reverseInclineLaunchedShuttle)
        {
            velocity = -velocity;
            FinishDeparting();
            return;
        }
        if (curRide->mode == RideMode::shuttle)
        {
            Flags ^= VehicleFlags::PoweredCarInReverse;
            velocity = 0;

            // We have turned, so treat it like entering a new tile
            UpdateCrossings();
        }
    }

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL)
    {
        sound2_flags |= VEHICLE_SOUND2_FLAGS_LIFT_HILL;
        if (curRide->mode != RideMode::reverseInclineLaunchedShuttle)
        {
            int32_t curSpeed = curRide->liftHillSpeed * 31079;
            if (velocity <= curSpeed)
            {
                acceleration = 15539;
                if (velocity != 0)
                {
                    if (_vehicleBreakdown == BREAKDOWN_SAFETY_CUT_OUT)
                    {
                        SetFlag(VehicleFlags::StoppedBySafetyCutOut);
                        ClearFlag(VehicleFlags::CollisionDisabled);
                    }
                }
                else
                    ClearFlag(VehicleFlags::CollisionDisabled);
            }
        }
        else
        {
            int32_t curSpeed = curRide->liftHillSpeed * -31079;
            if (velocity >= curSpeed)
            {
                acceleration = -15539;
                if (velocity != 0)
                {
                    if (_vehicleBreakdown == BREAKDOWN_SAFETY_CUT_OUT)
                    {
                        SetFlag(VehicleFlags::StoppedBySafetyCutOut);
                        ClearFlag(VehicleFlags::CollisionDisabled);
                    }
                }
                else
                    ClearFlag(VehicleFlags::CollisionDisabled);
            }
        }
    }

    if (curRide->mode == RideMode::freefallDrop)
    {
        animation_frame++;
    }
    else
    {
        bool shouldLaunch = true;
        if (curRide->mode == RideMode::downwardLaunch)
        {
            if (NumLaunches < 1)
                shouldLaunch = false;
        }

        if (shouldLaunch)
        {
            if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_3) || _vehicleStationIndex != current_station)
            {
                FinishDeparting();
                return;
            }

            if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_5))
                return;
            if (curRide->mode == RideMode::boatHire || curRide->mode == RideMode::rotatingLift
                || curRide->mode == RideMode::shuttle)
                return;

            UpdateCrashSetup();
            return;
        }
    }

    if (!CurrentTowerElementIsTop())
    {
        if (curRide->mode == RideMode::freefallDrop)
            Invalidate();
        return;
    }

    FinishDeparting();
}

/**
 * Part of UpdateDeparting
 * Called after finishing departing sequence to enter
 * traveling state.
 * Vertical rides class the lift to the top of the tower
 * as the departing sequence. After this point station
 * boosters do not affect the ride.
 *  rct2: 0x006D8858
 */
void Vehicle::FinishDeparting()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->mode == RideMode::downwardLaunch)
    {
        if (NumLaunches >= 1 && (14 << 16) > velocity)
            return;

        Play3D(SoundId::rideLaunch1, GetLocation());
    }

    if (curRide->mode == RideMode::upwardLaunch)
    {
        if ((curRide->launchSpeed << 16) > velocity)
            return;

        Play3D(SoundId::rideLaunch1, GetLocation());
    }

    if (curRide->mode != RideMode::race && !curRide->isBlockSectioned())
    {
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
    lost_time_out = 0;
    SetState(Status::travelling, 1);
    if (velocity < 0)
        sub_state = 0;
}

/**
 *
 *  rct2: 0x006DE5CB
 */
void Vehicle::CheckIfMissing()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->lifecycleFlags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        return;

    if (curRide->isBlockSectioned())
        return;

    if (!curRide->getRideTypeDescriptor().HasFlag(RtdFlag::checkForStalling))
        return;

    lost_time_out++;
    if (curRide->lifecycleFlags & RIDE_LIFECYCLE_HAS_STALLED_VEHICLE)
        return;

    uint16_t limit = curRide->type == RIDE_TYPE_BOAT_HIRE ? 15360 : 9600;

    if (lost_time_out <= limit)
        return;

    curRide->lifecycleFlags |= RIDE_LIFECYCLE_HAS_STALLED_VEHICLE;

    if (Config::Get().notifications.rideStalledVehicles)
    {
        Formatter ft;
        ft.Add<StringId>(GetRideComponentName(GetRideTypeDescriptor(curRide->type).NameConvention.vehicle).number);

        uint8_t vehicleIndex = 0;
        for (; vehicleIndex < curRide->numTrains; ++vehicleIndex)
            if (curRide->vehicles[vehicleIndex] == Id)
                break;

        vehicleIndex++;
        ft.Add<uint16_t>(vehicleIndex);
        curRide->formatNameTo(ft);
        ft.Add<StringId>(GetRideComponentName(GetRideTypeDescriptor(curRide->type).NameConvention.station).singular);

        News::AddItemToQueue(News::ItemType::ride, STR_NEWS_VEHICLE_HAS_STALLED, ride.ToUnderlying(), ft);
    }
}

void Vehicle::SimulateCrash() const
{
    auto curRide = GetRide();
    if (curRide != nullptr)
    {
        curRide->lifecycleFlags |= RIDE_LIFECYCLE_CRASHED;
    }
}

/**
 * Setup function for a vehicle colliding with
 * another vehicle.
 *
 *  rct2: 0x006DA059
 */
void Vehicle::UpdateCollisionSetup()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->status == RideStatus::simulating)
    {
        SimulateCrash();
        return;
    }

    SetState(Status::crashed, sub_state);

    if (!(curRide->lifecycleFlags & RIDE_LIFECYCLE_CRASHED))
    {
        auto frontVehicle = GetHead();
        auto trainIndex = ride_get_train_index_from_vehicle(*curRide, frontVehicle->Id);
        if (!trainIndex.has_value())
        {
            return;
        }

        curRide->crash(trainIndex.value());

        if (curRide->status != RideStatus::closed)
        {
            // We require this to execute right away during the simulation, always ignore network and queue.
            auto gameAction = GameActions::RideSetStatusAction(curRide->id, RideStatus::closed);
            GameActions::ExecuteNested(&gameAction, getGameState());
        }
    }

    curRide->lifecycleFlags |= RIDE_LIFECYCLE_CRASHED;
    curRide->windowInvalidateFlags.set(RideInvalidateFlag::main, RideInvalidateFlag::list);
    KillAllPassengersInTrain();

    Vehicle* lastVehicle = this;
    for (Vehicle* train = getGameState().entities.GetEntity<Vehicle>(Id); train != nullptr;
         train = getGameState().entities.GetEntity<Vehicle>(train->next_vehicle_on_train))
    {
        lastVehicle = train;

        train->sub_state = 2;

#ifdef ENABLE_SCRIPTING
        InvokeVehicleCrashHook(train->Id, "another_vehicle");
#endif
        const auto trainLoc = train->GetLocation();

        Play3D(SoundId::crash, trainLoc);

        ExplosionCloud::Create(trainLoc);

        for (int32_t i = 0; i < 10; i++)
        {
            VehicleCrashParticle::Create(train->colours, trainLoc);
        }

        train->SetFlag(VehicleFlags::Crashed);
        train->animationState = ScenarioRand() & 0xFFFF;

        train->animation_frame = ScenarioRand() & 0x7;
        train->SpriteData.Width = 13;
        train->SpriteData.HeightMin = 45;
        train->SpriteData.HeightMax = 5;

        train->MoveTo(trainLoc);

        train->SwingSpeed = 0;
    }

    // Remove the current train from the ride linked list of trains
    auto prevTrain = getGameState().entities.GetEntity<Vehicle>(prev_vehicle_on_ride);
    auto nextTrain = getGameState().entities.GetEntity<Vehicle>(lastVehicle->next_vehicle_on_ride);
    if (prevTrain == nullptr || nextTrain == nullptr)
    {
        LOG_ERROR("Corrupted vehicle list for ride!");
    }
    else
    {
        prevTrain->next_vehicle_on_ride = lastVehicle->next_vehicle_on_ride;
        nextTrain->prev_vehicle_on_ride = prev_vehicle_on_ride;
    }

    velocity = 0;
}

/**
 *
 *  rct2: 0x006D9EFE
 */
void Vehicle::UpdateCrashSetup()
{
    auto curRide = GetRide();
    if (curRide != nullptr && curRide->status == RideStatus::simulating)
    {
        SimulateCrash();
        return;
    }
    SetState(Status::crashing, sub_state);

    if (NumPeepsUntilTrainTail() != 0)
    {
        Play3D(SoundId::hauntedHouseScream2, GetLocation());
    }

    int32_t edx = velocity >> 10;

    Vehicle* lastVehicle = this;
    auto spriteId = Id;
    for (Vehicle* trainVehicle; !spriteId.IsNull(); spriteId = trainVehicle->next_vehicle_on_train)
    {
        trainVehicle = getGameState().entities.GetEntity<Vehicle>(spriteId);
        if (trainVehicle == nullptr)
        {
            break;
        }
        lastVehicle = trainVehicle;

        trainVehicle->sub_state = 0;
        auto crashDirection = Geometry::getCrashDirectionComponents(trainVehicle->Orientation);
        int32_t trainX = crashDirection.x;
        int32_t trainY = crashDirection.y;

        auto carLaunchDirection = Geometry::getPitchVector32(trainVehicle->pitch);

        auto trainZ = carLaunchDirection.y >> 23;
        int32_t ecx = carLaunchDirection.x >> 15;
        trainX *= ecx;
        trainY *= ecx;
        trainX >>= 16;
        trainY >>= 16;
        trainX *= edx;
        trainY *= edx;
        trainZ *= edx;
        trainX >>= 8;
        trainY >>= 8;
        trainZ >>= 8;

        trainVehicle->crash_x = trainX;
        trainVehicle->crash_y = trainY;
        trainVehicle->crash_z = trainZ;
        trainVehicle->crash_x += (ScenarioRand() & 0xF) - 8;
        trainVehicle->crash_y += (ScenarioRand() & 0xF) - 8;
        trainVehicle->crash_z += (ScenarioRand() & 0xF) - 8;

        trainVehicle->TrackLocation = { 0, 0, 0 };
    }

    // Remove the current train from the ride linked list of trains
    auto prevTrain = getGameState().entities.GetEntity<Vehicle>(prev_vehicle_on_ride);
    auto nextTrain = getGameState().entities.GetEntity<Vehicle>(lastVehicle->next_vehicle_on_ride);
    if (prevTrain == nullptr || nextTrain == nullptr)
    {
        LOG_ERROR("Corrupted vehicle list for ride!");
    }
    else
    {
        prevTrain->next_vehicle_on_ride = lastVehicle->next_vehicle_on_ride;
        nextTrain->prev_vehicle_on_ride = prev_vehicle_on_ride;
    }
    velocity = 0;
}

/**
 *
 *  rct2: 0x006D8937
 */
void Vehicle::UpdateTravelling()
{
    CheckIfMissing();

    auto curRide = GetRide();
    if (curRide == nullptr || (_vehicleBreakdown == 0 && curRide->mode == RideMode::rotatingLift))
        return;

    if (sub_state == 2)
    {
        velocity = 0;
        acceleration = 0;
        var_C0--;
        if (var_C0 == 0)
            sub_state = 0;
    }

    if (curRide->mode == RideMode::freefallDrop && animation_frame != 0)
    {
        animation_frame++;
        velocity = 0;
        acceleration = 0;
        Invalidate();
        return;
    }

    uint32_t curFlags = UpdateTrackMotion(nullptr);

    bool skipCheck = false;
    if (curFlags & (VEHICLE_UPDATE_MOTION_TRACK_FLAG_8 | VEHICLE_UPDATE_MOTION_TRACK_FLAG_9)
        && curRide->mode == RideMode::reverseInclineLaunchedShuttle && sub_state == 0)
    {
        sub_state = 1;
        velocity = 0;
        skipCheck = true;
    }

    if (!skipCheck)
    {
        if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_DERAILED)
        {
            UpdateCrashSetup();
            return;
        }

        if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION)
        {
            UpdateCollisionSetup();
            return;
        }

        if (curFlags & (VEHICLE_UPDATE_MOTION_TRACK_FLAG_5 | VEHICLE_UPDATE_MOTION_TRACK_FLAG_12))
        {
            if (curRide->mode == RideMode::rotatingLift)
            {
                if (sub_state <= 1)
                {
                    SetState(Status::arriving, 1);
                    var_C0 = 0;
                    return;
                }
            }
            else if (curRide->mode == RideMode::boatHire)
            {
                UpdateTravellingBoatHireSetup();
                return;
            }
            if (curRide->mode == RideMode::shuttle)
            {
                Flags ^= VehicleFlags::PoweredCarInReverse;
                velocity = 0;
            }
            else
            {
                if (sub_state != 0)
                {
                    UpdateCrashSetup();
                    return;
                }
                sub_state = 1;
                velocity = 0;
            }
        }
    }

    if (curRide->mode == RideMode::rotatingLift && sub_state <= 1)
    {
        if (sub_state == 0)
        {
            if (velocity >= -131940)
                acceleration = -3298;
            velocity = std::max(velocity, -131940);
        }
        else
        {
            if (CurrentTowerElementIsTop())
            {
                velocity = 0;
                sub_state = 2;
                var_C0 = 150;
            }
            else
            {
                if (velocity <= 131940)
                    acceleration = 3298;
            }
        }
    }

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL)
    {
        if (curRide->mode == RideMode::reverseInclineLaunchedShuttle)
        {
            if (sub_state == 0)
            {
                if (velocity != 0)
                    sound2_flags |= VEHICLE_SOUND2_FLAGS_LIFT_HILL;

                if (!HasFlag(VehicleFlags::ReverseInclineCompletedLap))
                {
                    if (velocity >= curRide->liftHillSpeed * -31079)
                    {
                        acceleration = -15539;

                        if (_vehicleBreakdown == 0)
                        {
                            sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
                            SetFlag(VehicleFlags::StoppedBySafetyCutOut);
                        }
                    }
                }
            }
        }
        else
        {
            sound2_flags |= VEHICLE_SOUND2_FLAGS_LIFT_HILL;
            if (velocity <= curRide->liftHillSpeed * 31079)
            {
                acceleration = 15539;
                if (velocity != 0)
                {
                    if (_vehicleBreakdown == 0)
                    {
                        SetFlag(VehicleFlags::StoppedBySafetyCutOut);
                        sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
                    }
                }
                else
                {
                    sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
                }
            }
        }
    }

    if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_3))
        return;

    if (curRide->mode == RideMode::reverseInclineLaunchedShuttle && velocity >= 0
        && !HasFlag(VehicleFlags::ReverseInclineCompletedLap))
    {
        return;
    }

    if (curRide->mode == RideMode::poweredLaunchPasstrough && velocity < 0)
        return;

    SetState(Status::arriving);
    current_station = _vehicleStationIndex;
    var_C0 = 0;
    if (velocity < 0)
        sub_state = 1;
}

void Vehicle::UpdateArrivingPassThroughStation(const Ride& curRide, const CarEntry& carEntry, bool stationBrakesWork)
{
    if (sub_state == 0)
    {
        if (curRide.mode == RideMode::race && curRide.lifecycleFlags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
        {
            return;
        }

        if (velocity <= 131940)
        {
            acceleration = 3298;
            return;
        }

        int32_t velocity_diff = velocity;
        if (velocity_diff >= 24.0_mph)
            velocity_diff /= 8;
        else
            velocity_diff /= 16;

        if (!stationBrakesWork)
        {
            return;
        }

        if (curRide.numCircuits != 1)
        {
            if (NumLaps + 1 < curRide.numCircuits)
            {
                return;
            }
        }
        velocity -= velocity_diff;
        acceleration = 0;
    }
    else
    {
        if (!carEntry.flags.has(CarEntryFlag::isPowered) && velocity >= -131940)
        {
            acceleration = -3298;
        }

        if (velocity >= -131940)
        {
            return;
        }

        int32_t velocity_diff = velocity;
        if (velocity_diff < -24.0_mph)
            velocity_diff /= 8;
        else
            velocity_diff /= 16;

        if (!stationBrakesWork)
        {
            return;
        }

        if (NumLaps + 1 < curRide.numCircuits)
        {
            return;
        }

        if (NumLaps + 1 != curRide.numCircuits)
        {
            velocity -= velocity_diff;
            acceleration = 0;
            return;
        }

        if (GetRideTypeDescriptor(curRide.type).HasFlag(RtdFlag::allowMultipleCircuits) && curRide.mode != RideMode::shuttle
            && curRide.mode != RideMode::poweredLaunch)
        {
            SetFlag(VehicleFlags::ReverseInclineCompletedLap);
        }
        else
        {
            velocity -= velocity_diff;
            acceleration = 0;
        }
    }
}

/**
 *
 *  rct2: 0x006D8C36
 */
void Vehicle::UpdateArriving()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    bool stationBrakesWork = true;
    uint32_t curFlags = 0;

    switch (curRide->mode)
    {
        case RideMode::swing:
        case RideMode::rotation:
        case RideMode::forwardRotation:
        case RideMode::backwardRotation:
        case RideMode::filmAvengingAviators:
        case RideMode::filmThrillRiders:
        case RideMode::beginners:
        case RideMode::intense:
        case RideMode::berserk:
        case RideMode::mouseTails3DFilm:
        case RideMode::stormChasers3DFilm:
        case RideMode::spaceRaiders3DFilm:
        case RideMode::circus:
        case RideMode::spaceRings:
        case RideMode::hauntedHouse:
        case RideMode::crookedHouse:
            ClearFlag(VehicleFlags::ReverseInclineCompletedLap);
            velocity = 0;
            acceleration = 0;
            SetState(Status::unloadingPassengers);
            return;
        default:
        {
            // This is workaround for multiple compilation errors of type "enumeration value ‘RIDE_MODE_*' not handled
            // in switch [-Werror=switch]"
        }
    }

    bool hasBrakesFailure = curRide->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN
        && curRide->breakdownReasonPending == BREAKDOWN_BRAKES_FAILURE;
    if (hasBrakesFailure && curRide->inspectionStation == current_station
        && curRide->mechanicStatus != MechanicStatus::hasFixedStationBrakes)
    {
        stationBrakesWork = false;
    }

    const auto* rideEntry = GetRideEntry();
    const auto& carEntry = rideEntry->Cars[vehicle_type];

    UpdateArrivingPassThroughStation(*curRide, carEntry, stationBrakesWork);

    curFlags = UpdateTrackMotion(nullptr);
    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION && !stationBrakesWork)
    {
        UpdateCollisionSetup();
        return;
    }

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION && !stationBrakesWork)
    {
        SetState(Status::departing, 1);
        return;
    }

    if (!(curFlags
          & (VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION | VEHICLE_UPDATE_MOTION_TRACK_FLAG_1
             | VEHICLE_UPDATE_MOTION_TRACK_FLAG_5)))
    {
        if (velocity > 98955)
            var_C0 = 0;
        return;
    }

    var_C0++;
    if ((curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_1) && carEntry.flags.has(CarEntryFlag::isGoKart) && (var_C0 < 40))
    {
        return;
    }

    auto trackElement = MapGetTrackElementAt(TrackLocation);

    if (trackElement == nullptr)
    {
        return;
    }

    current_station = trackElement->GetStationIndex();
    NumLaps++;

    if (sub_state != 0)
    {
        if (NumLaps < curRide->numCircuits)
        {
            SetState(Status::departing, 1);
            return;
        }

        if (NumLaps == curRide->numCircuits && HasFlag(VehicleFlags::ReverseInclineCompletedLap))
        {
            SetState(Status::departing, 1);
            return;
        }
    }

    if (curRide->numCircuits != 1 && NumLaps < curRide->numCircuits)
    {
        SetState(Status::departing, 1);
        return;
    }

    if ((curRide->mode == RideMode::upwardLaunch || curRide->mode == RideMode::downwardLaunch) && NumLaunches < 2)
    {
        Play3D(SoundId::rideLaunch2, GetLocation());
        velocity = 0;
        acceleration = 0;
        SetState(Status::departing, 1);
        return;
    }

    if (curRide->mode == RideMode::race && curRide->lifecycleFlags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
    {
        SetState(Status::departing, 1);
        return;
    }

    ClearFlag(VehicleFlags::ReverseInclineCompletedLap);
    velocity = 0;
    acceleration = 0;
    SetState(Status::unloadingPassengers);
}

/**
 *
 *  rct2: 0x006D9002
 */
void Vehicle::UpdateUnloadingPassengers()
{
    if (sub_state == 0)
    {
        if (OpenRestraints())
        {
            sub_state = 1;
        }
    }

    const auto* curRide = GetRide();
    if (curRide == nullptr)
        return;

    const auto& currentStation = curRide->getStation(current_station);

    if (curRide->mode == RideMode::forwardRotation || curRide->mode == RideMode::backwardRotation)
    {
        uint8_t seat = ((-flatRideAnimationFrame) >> 3) & 0xF;
        if (restraints_position == 255 && !peep[seat * 2].IsNull())
        {
            next_free_seat -= 2;

            auto firstGuest = getGameState().entities.GetEntity<Guest>(peep[seat * 2]);
            peep[seat * 2] = EntityId::GetNull();

            if (firstGuest != nullptr)
            {
                firstGuest->SetState(PeepState::leavingRide);
                firstGuest->RideSubState = PeepRideSubState::leaveVehicle;
            }

            auto secondGuest = getGameState().entities.GetEntity<Guest>(peep[seat * 2 + 1]);
            peep[seat * 2 + 1] = EntityId::GetNull();

            if (secondGuest != nullptr)
            {
                secondGuest->SetState(PeepState::leavingRide);
                secondGuest->RideSubState = PeepRideSubState::leaveVehicle;
            }
        }
    }
    else
    {
        if (currentStation.Exit.IsNull())
        {
            if (sub_state != 1)
                return;

            if (!(curRide->lifecycleFlags & RIDE_LIFECYCLE_TESTED) && HasFlag(VehicleFlags::Testing)
                && curRide->currentTestSegment + 1 >= curRide->numStations)
            {
                UpdateTestFinish();
            }
            SetState(Status::movingToEndOfStation);
            return;
        }

        for (Vehicle* train = getGameState().entities.GetEntity<Vehicle>(Id); train != nullptr;
             train = getGameState().entities.GetEntity<Vehicle>(train->next_vehicle_on_train))
        {
            if (train->restraints_position != 255)
                continue;

            if (train->next_free_seat == 0)
                continue;

            train->next_free_seat = 0;
            for (uint8_t peepIndex = 0; peepIndex < train->num_peeps; peepIndex++)
            {
                Peep* curPeep = getGameState().entities.GetEntity<Guest>(train->peep[peepIndex]);
                if (curPeep != nullptr)
                {
                    curPeep->SetState(PeepState::leavingRide);
                    curPeep->RideSubState = PeepRideSubState::leaveVehicle;
                }
            }
        }
    }

    if (sub_state != 1)
        return;

    for (Vehicle* train = getGameState().entities.GetEntity<Vehicle>(Id); train != nullptr;
         train = getGameState().entities.GetEntity<Vehicle>(train->next_vehicle_on_train))
    {
        if (train->num_peeps != train->next_free_seat)
            return;
    }

    if (!(curRide->lifecycleFlags & RIDE_LIFECYCLE_TESTED) && HasFlag(VehicleFlags::Testing)
        && curRide->currentTestSegment + 1 >= curRide->numStations)
    {
        UpdateTestFinish();
    }
    SetState(Status::movingToEndOfStation);
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
 *  rct2: 0x006D9820
 */
void Vehicle::UpdateTravellingBoat()
{
    CheckIfMissing();
    UpdateMotionBoatHire();
}

void Vehicle::TryReconnectBoatToTrack(const CoordsXY& currentBoatLocation, const CoordsXY& trackCoords)
{
    remaining_distance = 0;
    if (!UpdateMotionCollisionDetection({ currentBoatLocation, z }, nullptr))
    {
        TrackLocation.x = trackCoords.x;
        TrackLocation.y = trackCoords.y;

        auto curRide = GetRide();
        if (curRide != nullptr)
        {
            auto trackElement = MapGetTrackElementAt(TrackLocation);
            if (trackElement != nullptr)
                SetTrackType(trackElement->GetTrackType());

            SetTrackDirection(curRide->boatHireReturnDirection);
            BoatLocation.SetNull();
        }

        track_progress = 0;
        SetState(Status::travelling, sub_state);
        _vehicleCurPosition.x = currentBoatLocation.x;
        _vehicleCurPosition.y = currentBoatLocation.y;
    }
}

/**
 *
 *  rct2: 0x006DA717
 */
void Vehicle::UpdateMotionBoatHire()
{
    _vehicleMotionTrackFlags = 0;
    velocity += acceleration;
    _vehicleVelocityF64E08 = velocity;
    _vehicleVelocityF64E0C = (velocity >> 10) * 42;

    auto carEntry = Entry();
    if (carEntry == nullptr)
    {
        return;
    }
    if (carEntry->flags.hasAny(CarEntryFlag::hasVehicleAnimation, CarEntryFlag::hasRiderAnimation))
    {
        UpdateAdditionalAnimation();
    }

    _vehicleUnkF64E10 = 1;
    acceleration = 0;
    remaining_distance += _vehicleVelocityF64E0C;
    if (remaining_distance >= 0x368A)
    {
        sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
        _vehicleCurPosition = GetLocation();
        Invalidate();

        for (;;)
        {
            // Loc6DA7A5
            var_35++;
            auto loc = BoatLocation.ToTileCentre();
            CoordsXY loc2 = loc;
            uint8_t bl;

            loc2.x -= x;
            if (loc2.x >= 0)
            {
                loc2.y -= y;
                if (loc2.y < 0)
                {
                    // Loc6DA81A:
                    loc2.y = -loc2.y;
                    bl = 24;
                    if (loc2.y <= loc2.x * 4)
                    {
                        bl = 16;
                        if (loc2.x <= loc2.y * 4)
                        {
                            bl = 20;
                        }
                    }
                }
                else
                {
                    bl = 8;
                    if (loc2.y <= loc2.x * 4)
                    {
                        bl = 16;
                        if (loc2.x <= loc2.y * 4)
                        {
                            bl = 12;
                        }
                    }
                }
            }
            else
            {
                loc2.y -= y;
                if (loc2.y < 0)
                {
                    // Loc6DA83D:
                    loc2.x = -loc2.x;
                    loc2.y = -loc2.y;
                    bl = 24;
                    if (loc2.y <= loc2.x * 4)
                    {
                        bl = 0;
                        if (loc2.x <= loc2.y * 4)
                        {
                            bl = 28;
                        }
                    }
                }
                else
                {
                    loc2.x = -loc2.x;
                    bl = 8;
                    if (loc2.y <= loc2.x * 4)
                    {
                        bl = 0;
                        if (loc2.x <= loc2.y * 4)
                        {
                            bl = 4;
                        }
                    }
                }
            }

            // Loc6DA861:
            var_34 = bl;
            loc2.x += loc2.y;
            if (loc2.x <= 12)
            {
                UpdateBoatLocation();
            }

            if (!(var_35 & (1 << 0)))
            {
                uint8_t spriteDirection = Orientation;
                if (spriteDirection != var_34)
                {
                    uint8_t dl = (var_34 + 16 - spriteDirection) & 0x1E;
                    if (dl >= 16)
                    {
                        spriteDirection += 2;
                        if (dl > 24)
                        {
                            var_35--;
                        }
                    }
                    else
                    {
                        spriteDirection -= 2;
                        if (dl < 8)
                        {
                            var_35--;
                        }
                    }

                    Orientation = spriteDirection & 0x1E;
                }
            }

            int32_t edi = (Orientation | (var_35 & 1)) & 0x1F;
            loc2 = { x + Geometry::getFreeroamVehicleMovementData(edi).x, y + Geometry::getFreeroamVehicleMovementData(edi).y };
            if (UpdateMotionCollisionDetection({ loc2, z }, nullptr))
            {
                remaining_distance = 0;
                if (Orientation == var_34)
                {
                    Orientation ^= (1 << 4);
                    UpdateBoatLocation();
                    Orientation ^= (1 << 4);
                }
                break;
            }

            auto flooredLocation = loc2.ToTileStart();
            if (flooredLocation != TrackLocation)
            {
                if (!vehicle_boat_is_location_accessible({ loc2, TrackLocation.z }))
                {
                    // Loc6DA939:
                    auto curRide = GetRide();
                    if (curRide == nullptr)
                        return;

                    bool do_Loc6DAA97 = false;
                    if (sub_state != BoatHireSubState::EnteringReturnPosition)
                    {
                        do_Loc6DAA97 = true;
                    }
                    else
                    {
                        auto flooredTileLoc = TileCoordsXY(flooredLocation);
                        if (curRide->boatHireReturnPosition != flooredTileLoc)
                        {
                            do_Loc6DAA97 = true;
                        }
                    }

                    // Loc6DAA97:
                    if (do_Loc6DAA97)
                    {
                        remaining_distance = 0;
                        if (Orientation == var_34)
                        {
                            UpdateBoatLocation();
                        }
                        break;
                    }

                    if (!(curRide->boatHireReturnDirection & 1))
                    {
                        uint16_t tilePart = loc2.y % kCoordsXYStep;
                        if (tilePart == kCoordsXYHalfTile)
                        {
                            TryReconnectBoatToTrack(loc2, flooredLocation);
                            break;
                        }
                        loc2 = _vehicleCurPosition;
                        if (tilePart <= kCoordsXYHalfTile)
                        {
                            loc2.y += 1;
                        }
                        else
                        {
                            loc2.y -= 1;
                        }
                    }
                    else
                    {
                        // Loc6DA9A2:
                        uint16_t tilePart = loc2.x % kCoordsXYStep;
                        if (tilePart == kCoordsXYHalfTile)
                        {
                            TryReconnectBoatToTrack(loc2, flooredLocation);
                            break;
                        }
                        loc2 = _vehicleCurPosition;
                        if (tilePart <= kCoordsXYHalfTile)
                        {
                            loc2.x += 1;
                        }
                        else
                        {
                            loc2.x -= 1;
                        }
                    }

                    // Loc6DA9D1:
                    remaining_distance = 0;
                    if (!UpdateMotionCollisionDetection({ loc2, z }, nullptr))
                    {
                        _vehicleCurPosition.x = loc2.x;
                        _vehicleCurPosition.y = loc2.y;
                    }
                    break;
                }
                TrackLocation = { flooredLocation, TrackLocation.z };
            }

            remaining_distance -= Geometry::getFreeroamVehicleMovementData(edi).distance;
            _vehicleCurPosition.x = loc2.x;
            _vehicleCurPosition.y = loc2.y;
            if (remaining_distance < 0x368A)
            {
                break;
            }
            _vehicleUnkF64E10++;
        }

        MoveTo(_vehicleCurPosition);
    }

    // Loc6DAAC9:
    {
        int32_t edx = velocity >> 8;
        edx = (edx * edx);
        if (velocity < 0)
        {
            edx = -edx;
        }
        edx >>= 5;

        // Hack to fix people messing with boat hire
        int32_t curMass = mass == 0 ? 1 : mass;

        int32_t eax = ((velocity >> 1) + edx) / curMass;
        int32_t newAcceleration = -eax;
        if (carEntry->flags.has(CarEntryFlag::isPowered))
        {
            eax = speed << 14;
            int32_t ebx = (speed * curMass) >> 2;
            if (HasFlag(VehicleFlags::PoweredCarInReverse))
            {
                eax = -eax;
            }
            eax -= velocity;
            edx = powered_acceleration * 2;
            if (ebx != 0)
            {
                newAcceleration += (eax * edx) / ebx;
            }
        }
        acceleration = newAcceleration;
    }
    // eax = _vehicleMotionTrackFlags;
    // ebx = _vehicleStationIndex;
}

/**
 *
 *  rct2: 0x006DA280
 */
void Vehicle::UpdateBoatLocation()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    TileCoordsXY returnPosition = curRide->boatHireReturnPosition;
    uint8_t returnDirection = curRide->boatHireReturnDirection & 3;

    CoordsXY location = CoordsXY{ x, y } + CoordsDirectionDelta[returnDirection];

    if (location.ToTileStart() == returnPosition.ToCoordsXY())
    {
        sub_state = BoatHireSubState::EnteringReturnPosition;
        BoatLocation = location.ToTileStart();
        return;
    }

    sub_state = BoatHireSubState::Normal;
    uint8_t curDirection = ((Orientation + 19) >> 3) & 3;
    uint8_t randDirection = ScenarioRand() & 3;

    if (lost_time_out > 1920)
    {
        if (ScenarioRand() & 1)
        {
            CoordsXY destLocation = (returnPosition.ToCoordsXY() - CoordsDirectionDelta[returnDirection]).ToTileCentre();

            destLocation.x -= x;
            destLocation.y -= y;

            if (abs(destLocation.x) <= abs(destLocation.y))
            {
                randDirection = destLocation.y < 0 ? 3 : 1;
            }
            else
            {
                randDirection = destLocation.x < 0 ? 0 : 2;
            }
        }
    }

    static constexpr int8_t rotations[] = { 0, 1, -1, 2 };
    for (auto rotation : rotations)
    {
        if (randDirection + rotation == curDirection)
        {
            continue;
        }

        auto trackLocation = TrackLocation;
        trackLocation += CoordsDirectionDelta[(randDirection + rotation) & 3];

        if (!vehicle_boat_is_location_accessible(trackLocation))
        {
            continue;
        }

        BoatLocation = trackLocation.ToTileStart();
        return;
    }

    CoordsXY trackLocation = TrackLocation;
    trackLocation += CoordsDirectionDelta[curDirection & 3];
    BoatLocation = trackLocation.ToTileStart();
}

/**
 *
 *  rct2: 0x006DA22A
 */
static bool vehicle_boat_is_location_accessible(const CoordsXYZ& location)
{
    TileElement* tileElement = MapGetFirstElementAt(location);
    if (tileElement == nullptr)
        return false;
    do
    {
        if (tileElement->IsGhost())
            continue;

        if (tileElement->GetType() == TileElementType::Surface)
        {
            int32_t waterZ = tileElement->AsSurface()->GetWaterHeight();
            if (location.z != waterZ)
            {
                return false;
            }
        }
        else
        {
            if (location.z > (tileElement->GetBaseZ() - (2 * kCoordsZStep))
                && location.z < tileElement->GetClearanceZ() + (2 * kCoordsZStep))
            {
                return false;
            }
        }
    } while (!(tileElement++)->IsLastForTile());
    return true;
}

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
    if (rideEntry->flags & RIDE_ENTRY_FLAG_INVERTER_SHIP_SWING_MODE)
    {
        swingState += 4;
        if (rideEntry->flags & RIDE_ENTRY_FLAG_MAGIC_CARPET_SWING_MODE)
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
    if (rideEntry->flags & RIDE_ENTRY_FLAG_TWIST_ROTATION_TYPE)
    {
        timeToSpriteMap = kTwistTimeToSpriteMaps[sub_state];
    }
    else if (rideEntry->flags & RIDE_ENTRY_FLAG_ENTERPRISE_ROTATION_TYPE)
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

/**
 *
 *  rct2: 0x0068B8BD
 * @returns the map element that the vehicle will collide with or NULL if no collisions.
 */
static TileElement* vehicle_check_collision(const CoordsXYZ& vehiclePosition)
{
    TileElement* tileElement = MapGetFirstElementAt(vehiclePosition);
    if (tileElement == nullptr)
    {
        return nullptr;
    }

    uint8_t quadrant;
    if ((vehiclePosition.x & 0x1F) >= 16)
    {
        quadrant = 1;
        if ((vehiclePosition.y & 0x1F) < 16)
            quadrant = 2;
    }
    else
    {
        quadrant = 4;
        if ((vehiclePosition.y & 0x1F) >= 16)
            quadrant = 8;
    }

    do
    {
        if (vehiclePosition.z < tileElement->GetBaseZ())
            continue;

        if (vehiclePosition.z >= tileElement->GetClearanceZ())
            continue;

        if (tileElement->GetOccupiedQuadrants() & quadrant)
            return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

static void ride_train_crash(Ride& ride, uint16_t numFatalities)
{
    Formatter ft;
    ft.Add<uint16_t>(numFatalities);

    uint8_t crashType = numFatalities == 0 ? RIDE_CRASH_TYPE_NO_FATALITIES : RIDE_CRASH_TYPE_FATALITIES;

    if (crashType >= ride.lastCrashType)
        ride.lastCrashType = crashType;

    if (numFatalities != 0)
    {
        if (Config::Get().notifications.rideCasualties)
        {
            ride.formatNameTo(ft);
            News::AddItemToQueue(
                News::ItemType::ride, numFatalities == 1 ? STR_X_PERSON_DIED_ON_X : STR_X_PEOPLE_DIED_ON_X,
                ride.id.ToUnderlying(), ft);
        }

        // TODO: get park id from ride/vehicle
        auto& park = getGameState().park;
        if (park.ratingCasualtyPenalty < 500)
        {
            park.ratingCasualtyPenalty += 200;
        }
    }
}
/**
 *
 *  rct2: 0x006DE6C6
 */
void Vehicle::KillAllPassengersInTrain()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    ride_train_crash(*curRide, NumPeepsUntilTrainTail());

    for (Vehicle* trainCar = getGameState().entities.GetEntity<Vehicle>(Id); trainCar != nullptr;
         trainCar = getGameState().entities.GetEntity<Vehicle>(trainCar->next_vehicle_on_train))
    {
        trainCar->KillPassengers(*curRide);
    }
}

void Vehicle::KillPassengers(const Ride& curRide)
{
    if (num_peeps != next_free_seat)
        return;

    if (num_peeps == 0)
        return;

    for (auto i = 0; i < num_peeps; i++)
    {
        auto* curPeep = getGameState().entities.GetEntity<Guest>(peep[i]);
        if (curPeep == nullptr)
            continue;

        if (!curPeep->OutsideOfPark)
        {
            DecrementGuestsInPark();
            auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
            ContextBroadcastIntent(&intent);
        }
        PeepEntityRemove(curPeep);
    }

    num_peeps = 0;
    next_free_seat = 0;
}

void Vehicle::CrashOnLand()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->status == RideStatus::simulating)
    {
        SimulateCrash();
        return;
    }
    SetState(Status::crashed, sub_state);

#ifdef ENABLE_SCRIPTING
    InvokeVehicleCrashHook(Id, "land");
#endif

    if (!(curRide->lifecycleFlags & RIDE_LIFECYCLE_CRASHED))
    {
        auto frontVehicle = GetHead();
        auto trainIndex = ride_get_train_index_from_vehicle(*curRide, frontVehicle->Id);
        if (!trainIndex.has_value())
        {
            return;
        }

        curRide->crash(trainIndex.value());

        if (curRide->status != RideStatus::closed)
        {
            // We require this to execute right away during the simulation, always ignore network and queue.
            auto gameAction = GameActions::RideSetStatusAction(curRide->id, RideStatus::closed);
            GameActions::ExecuteNested(&gameAction, getGameState());
        }
    }
    curRide->lifecycleFlags |= RIDE_LIFECYCLE_CRASHED;
    curRide->windowInvalidateFlags.set(RideInvalidateFlag::main, RideInvalidateFlag::list);

    if (IsHead())
    {
        KillAllPassengersInTrain();
    }

    sub_state = 2;

    const auto curLoc = GetLocation();
    Play3D(SoundId::crash, curLoc);

    ExplosionCloud::Create(curLoc);
    ExplosionFlare::Create(curLoc);

    uint8_t numParticles = std::min(SpriteData.Width, static_cast<uint8_t>(7));

    while (numParticles-- != 0)
        VehicleCrashParticle::Create(colours, curLoc);

    SetFlag(VehicleFlags::Crashed);
    animation_frame = 0;
    animationState = 0;
    SpriteData.Width = 13;
    SpriteData.HeightMin = 45;
    SpriteData.HeightMax = 5;

    MoveTo(curLoc);

    crash_z = 0;
}

void Vehicle::CrashOnWater()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->status == RideStatus::simulating)
    {
        SimulateCrash();
        return;
    }
    SetState(Status::crashed, sub_state);

#ifdef ENABLE_SCRIPTING
    InvokeVehicleCrashHook(Id, "water");
#endif

    if (!(curRide->lifecycleFlags & RIDE_LIFECYCLE_CRASHED))
    {
        auto frontVehicle = GetHead();
        auto trainIndex = ride_get_train_index_from_vehicle(*curRide, frontVehicle->Id);
        if (!trainIndex.has_value())
        {
            return;
        }

        curRide->crash(trainIndex.value());

        if (curRide->status != RideStatus::closed)
        {
            // We require this to execute right away during the simulation, always ignore network and queue.
            auto gameAction = GameActions::RideSetStatusAction(curRide->id, RideStatus::closed);
            GameActions::ExecuteNested(&gameAction, getGameState());
        }
    }
    curRide->lifecycleFlags |= RIDE_LIFECYCLE_CRASHED;
    curRide->windowInvalidateFlags.set(RideInvalidateFlag::main, RideInvalidateFlag::list);

    if (IsHead())
    {
        KillAllPassengersInTrain();
    }

    sub_state = 2;

    const auto curLoc = GetLocation();
    Play3D(SoundId::water1, curLoc);

    CrashSplashParticle::Create(curLoc);
    CrashSplashParticle::Create(curLoc + CoordsXYZ{ -8, -9, 0 });
    CrashSplashParticle::Create(curLoc + CoordsXYZ{ 11, -9, 0 });
    CrashSplashParticle::Create(curLoc + CoordsXYZ{ 11, 8, 0 });
    CrashSplashParticle::Create(curLoc + CoordsXYZ{ -4, 8, 0 });

    for (int32_t i = 0; i < 10; ++i)
        VehicleCrashParticle::Create(colours, curLoc + CoordsXYZ{ -4, 8, 0 });

    SetFlag(VehicleFlags::Crashed);
    animation_frame = 0;
    animationState = 0;
    SpriteData.Width = 13;
    SpriteData.HeightMin = 45;
    SpriteData.HeightMax = 5;

    MoveTo(curLoc);

    crash_z = -1;
}

/**
 *
 *  rct2: 0x006D98CA
 */
void Vehicle::UpdateCrash()
{
    for (Vehicle* curVehicle = getGameState().entities.GetEntity<Vehicle>(Id); curVehicle != nullptr;
         curVehicle = getGameState().entities.GetEntity<Vehicle>(curVehicle->next_vehicle_on_train))
    {
        CoordsXYZ curPos = curVehicle->GetLocation();

        if (curVehicle->sub_state > 1)
        {
            if (curVehicle->crash_z <= 96)
            {
                curVehicle->crash_z++;
                if ((ScenarioRand() & 0xFFFF) <= 0x1555)
                {
                    int32_t xOffset = (ScenarioRand() & 2) - 1;
                    int32_t yOffset = (ScenarioRand() & 2) - 1;

                    ExplosionCloud::Create(curPos + CoordsXYZ{ xOffset, yOffset, 0 });
                }
            }
            if (curVehicle->animationState <= 0xe388)
            {
                curVehicle->animationState += 0x1c71;
            }
            else
            {
                curVehicle->animationState = 0;
                curVehicle->animation_frame++;
                if (curVehicle->animation_frame >= 8)
                    curVehicle->animation_frame = 0;
                curVehicle->Invalidate();
            }
            continue;
        }

        TileElement* collideElement = vehicle_check_collision(curPos);
        if (collideElement == nullptr)
        {
            curVehicle->sub_state = 1;
        }
        else if (curVehicle->sub_state == 1)
        {
            curVehicle->CrashOnLand();
            continue;
        }

        int16_t height = TileElementHeight(curPos);
        int16_t waterHeight = TileElementWaterHeight(curPos);
        int16_t zDiff;
        if (waterHeight != 0)
        {
            zDiff = curPos.z - waterHeight;
            if (zDiff <= 0 && zDiff >= -20)
            {
                curVehicle->CrashOnWater();
                continue;
            }
        }

        zDiff = curPos.z - height;
        if ((zDiff <= 0 && zDiff >= -20) || curPos.z < 16)
        {
            curVehicle->CrashOnLand();
            continue;
        }

        curVehicle->Invalidate();

        curPos.x += static_cast<int8_t>(curVehicle->crash_x >> 8);
        curPos.y += static_cast<int8_t>(curVehicle->crash_y >> 8);
        curPos.z += static_cast<int8_t>(curVehicle->crash_z >> 8);
        curVehicle->TrackLocation = { (curVehicle->crash_x << 8), (curVehicle->crash_y << 8), (curVehicle->crash_z << 8) };

        if (!MapIsLocationValid(curPos))
        {
            curVehicle->CrashOnLand();
            continue;
        }

        curVehicle->MoveTo(curPos);

        if (curVehicle->sub_state == 1)
        {
            curVehicle->crash_z -= 20;
        }
    }
}
/**
 *
 *  rct2: 0x006D7888
 */
void Vehicle::UpdateSound()
{
    // frictionVolume (bl) should be set before hand
    SoundIdVolume frictionSound = { SoundId::null, 255 };
    // bh screamVolume should be set before hand
    SoundIdVolume screamSound = { SoundId::null, 255 };

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    auto rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
        return;

    // Always use the head car's sound data (Some of the other vehicle subtypes have improperly set data)
    auto soundCarIndex = (rideEntry->FrontCar == 0xff) ? rideEntry->DefaultCar : rideEntry->FrontCar;
    const auto& carEntry = rideEntry->Cars[soundCarIndex];

    int32_t ecx = abs(velocity) - 1.0_mph;
    if (ecx >= 0)
    {
        frictionSound.id = carEntry.friction_sound_id;
        ecx >>= 15;
        frictionSound.volume = std::min(208 + (ecx & 0xFF), 255);
    }

    const auto currentTicks = getGameState().currentTicks;
    switch (carEntry.soundRange)
    {
        case SoundRange::steamWhistle:
            screamSound.id = scream_sound_id;
            if (!(currentTicks & 0x7F))
            {
                if (velocity < 4.0_mph || scream_sound_id != SoundId::null)
                {
                    GetLiftHillSound(*curRide, screamSound);
                    break;
                }

                if ((ScenarioRand() & 0xFFFF) <= 0x5555)
                {
                    scream_sound_id = SoundId::trainWhistle;
                    screamSound.volume = 255;
                    break;
                }
            }
            if (screamSound.id == SoundId::noScream)
                screamSound.id = SoundId::null;
            screamSound.volume = 255;
            break;

        case SoundRange::tramBell:
            screamSound.id = scream_sound_id;
            if (!(currentTicks & 0x7F))
            {
                if (velocity < 4.0_mph || scream_sound_id != SoundId::null)
                {
                    GetLiftHillSound(*curRide, screamSound);
                    break;
                }

                if ((ScenarioRand() & 0xFFFF) <= 0x5555)
                {
                    scream_sound_id = SoundId::tram;
                    screamSound.volume = 255;
                    break;
                }
            }
            if (screamSound.id == SoundId::noScream)
                screamSound.id = SoundId::null;
            screamSound.volume = 255;
            break;

        default:
            if (carEntry.flags.has(CarEntryFlag::hasScreamingRiders))
            {
                screamSound.id = UpdateScreamSound();
                if (screamSound.id == SoundId::noScream)
                {
                    screamSound.id = SoundId::null;
                    break;
                }
                if (screamSound.id != SoundId::null)
                {
                    break;
                }
            }
            GetLiftHillSound(*curRide, screamSound);
    }

    // Friction sound
    auto soundIdVolume = VehicleSoundFadeInOut(sound1_id, sound1_volume, frictionSound.id, frictionSound.volume);
    sound1_id = soundIdVolume.id;
    sound1_volume = soundIdVolume.volume;

    // Scream sound
    soundIdVolume = VehicleSoundFadeInOut(sound2_id, sound2_volume, screamSound.id, screamSound.volume);
    sound2_id = soundIdVolume.id;
    sound2_volume = soundIdVolume.volume;

    // Calculate Sound Vector (used for sound frequency calcs)
    int32_t soundDirection = Geometry::getSoundDirectionFromOrientation(Orientation);
    int32_t soundVector = ((velocity >> 14) * soundDirection) >> 14;
    soundVector = std::clamp(soundVector, -127, 127);

    dopplerShift = soundVector & 0xFF;
}

/**
 *
 *  rct2: 0x006D796B
 */
SoundId Vehicle::UpdateScreamSound()
{
    int32_t totalNumPeeps = NumPeepsUntilTrainTail();
    if (totalNumPeeps == 0)
        return SoundId::null;

    if (velocity < 0)
    {
        if (velocity > -2.75_mph)
            return SoundId::null;

        for (Vehicle* vehicle2 = getGameState().entities.GetEntity<Vehicle>(Id); vehicle2 != nullptr;
             vehicle2 = getGameState().entities.GetEntity<Vehicle>(vehicle2->next_vehicle_on_train))
        {
            if (vehicle2->pitch < VehiclePitch::up12)
                continue;
            if (vehicle2->pitch <= VehiclePitch::up60)
                return ProduceScreamSound(totalNumPeeps);
            if (vehicle2->pitch < VehiclePitch::up75)
                continue;
            if (vehicle2->pitch <= VehiclePitch::up165)
                return ProduceScreamSound(totalNumPeeps);
            // up50 occurs on diagonal steep hills
            // up8 and up16 occur on diagonal gentle hills
            if (vehicle2->pitch == VehiclePitch::up50)
                return ProduceScreamSound(totalNumPeeps);
        }
        return SoundId::null;
    }

    if (velocity < 2.75_mph)
        return SoundId::null;

    for (Vehicle* vehicle2 = getGameState().entities.GetEntity<Vehicle>(Id); vehicle2 != nullptr;
         vehicle2 = getGameState().entities.GetEntity<Vehicle>(vehicle2->next_vehicle_on_train))
    {
        if (vehicle2->pitch < VehiclePitch::down12)
            continue;
        if (vehicle2->pitch <= VehiclePitch::down60)
            return ProduceScreamSound(totalNumPeeps);
        if (vehicle2->pitch <= VehiclePitch::inverted)
            continue;
        if (vehicle2->pitch <= VehiclePitch::down165)
            return ProduceScreamSound(totalNumPeeps);
        // down50 occurs on diagonal steep drops
        // down8 and down16 occur on diagonal gentle drops
        if (vehicle2->pitch == VehiclePitch::down50)
            return ProduceScreamSound(totalNumPeeps);
    }
    return SoundId::null;
}

SoundId Vehicle::ProduceScreamSound(const int32_t totalNumPeeps)
{
    const auto* rideEntry = GetRideEntry();

    const auto& carEntry = rideEntry->Cars[vehicle_type];

    if (scream_sound_id == SoundId::null)
    {
        auto r = ScenarioRand();
        if (totalNumPeeps >= static_cast<int32_t>(r % 16))
        {
            switch (carEntry.soundRange)
            {
                case SoundRange::screamsMisc:
                    scream_sound_id = _screamSetMisc[r % std::size(_screamSetMisc)];
                    break;
                case SoundRange::screamsWoodenRollerCoaster:
                    scream_sound_id = _screamSetWooden[r % std::size(_screamSetWooden)];
                    break;
                case SoundRange::screamSteelRollerCoaster:
                    scream_sound_id = _screamSetSteel[r % std::size(_screamSetSteel)];
                    break;
                default:
                    scream_sound_id = SoundId::noScream;
                    break;
            }
        }
        else
        {
            scream_sound_id = SoundId::noScream;
        }
    }
    return scream_sound_id;
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
    if (HasFlag(VehicleFlags::CarIsReversed))
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

        if (HasFlag(VehicleFlags::OnLiftHill))
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
    if (HasFlag(VehicleFlags::SpinningIsLocked))
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
    if (currentRide == nullptr || !currentRide->getRideTypeDescriptor().HasFlag(RtdFlag::hasLandscapeDoors))
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

    if (!(rideEntry->flags & RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND))
    {
        if (rideEntry->flags & RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND_SLIDE)
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
            if (GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::hasInvertedVariant))
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
        else if (rideEntry.flags & RIDE_ENTRY_FLAG_RIDER_CONTROLS_SPEED && num_peeps > 0)
        {
            acceleration += CalculateRiderBraking();
        }

        if ((trackType == TrackElemType::flat && curRide.getRideTypeDescriptor().HasFlag(RtdFlag::hasLsmBehaviourOnFlat))
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
                        if (!(rideEntry.flags & RIDE_ENTRY_FLAG_DISABLE_COLLISION_CRASHES))
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
        if (GetRideTypeDescriptor(curRide.type).HasFlag(RtdFlag::hasInvertedVariant))
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
        if (trackType == TrackElemType::flat && curRide.getRideTypeDescriptor().HasFlag(RtdFlag::hasLsmBehaviourOnFlat))
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

                        if (!(rideEntry.flags & RIDE_ENTRY_FLAG_DISABLE_COLLISION_CRASHES))
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

[[nodiscard]] Vehicle::UpdateMiniGolfSubroutineStatus Vehicle::UpdateTrackMotionMiniGolfFlagsStatus(const Ride& curRide)
{
    if (var_D3 != 0)
    {
        var_D3--;
        remaining_distance -= 0x368A;
        if (remaining_distance < 0)
        {
            remaining_distance = 0;
        }

        if (remaining_distance < 0x368A)
        {
            Loc6DCDE4(curRide);
            return UpdateMiniGolfSubroutineStatus::stop;
        }
        acceleration = Geometry::getAccelerationFromPitch(pitch);
        _vehicleUnkF64E10++;
        return UpdateMiniGolfSubroutineStatus::restart;
    }

    if (mini_golf_flags & MiniGolfFlag::Flag2)
    {
        uint8_t nextFrame = animation_frame + 1;
        if (nextFrame < kMiniGolfPeepAnimationLengths[EnumValue(mini_golf_current_animation)])
        {
            animation_frame = nextFrame;
            remaining_distance -= 0x368A;
            if (remaining_distance < 0)
            {
                remaining_distance = 0;
            }

            if (remaining_distance < 0x368A)
            {
                Loc6DCDE4(curRide);
                return UpdateMiniGolfSubroutineStatus::stop;
            }
            acceleration = Geometry::getAccelerationFromPitch(pitch);
            _vehicleUnkF64E10++;
            return UpdateMiniGolfSubroutineStatus::restart;
        }
        mini_golf_flags &= ~MiniGolfFlag::Flag2;
    }

    if (mini_golf_flags & MiniGolfFlag::Flag0)
    {
        auto vehicleIdx = IsHead() ? next_vehicle_on_ride : prev_vehicle_on_ride;
        Vehicle* vEDI = getGameState().entities.GetEntity<Vehicle>(vehicleIdx);
        if (vEDI == nullptr)
        {
            return UpdateMiniGolfSubroutineStatus::stop;
        }
        if (!(vEDI->mini_golf_flags & MiniGolfFlag::Flag0) || (vEDI->mini_golf_flags & MiniGolfFlag::Flag2))
        {
            remaining_distance -= 0x368A;
            if (remaining_distance < 0)
            {
                remaining_distance = 0;
            }

            if (remaining_distance < 0x368A)
            {
                Loc6DCDE4(curRide);
                return UpdateMiniGolfSubroutineStatus::stop;
            }
            acceleration = Geometry::getAccelerationFromPitch(pitch);
            _vehicleUnkF64E10++;
            return UpdateMiniGolfSubroutineStatus::restart;
        }
        if (vEDI->var_D3 != 0)
        {
            remaining_distance -= 0x368A;
            if (remaining_distance < 0)
            {
                remaining_distance = 0;
            }

            if (remaining_distance < 0x368A)
            {
                Loc6DCDE4(curRide);
                return UpdateMiniGolfSubroutineStatus::stop;
            }
            acceleration = Geometry::getAccelerationFromPitch(pitch);
            _vehicleUnkF64E10++;
            return UpdateMiniGolfSubroutineStatus::restart;
        }
        vEDI->mini_golf_flags &= ~MiniGolfFlag::Flag0;
        mini_golf_flags &= ~MiniGolfFlag::Flag0;
    }

    if (mini_golf_flags & MiniGolfFlag::Flag1)
    {
        auto vehicleIdx = IsHead() ? next_vehicle_on_ride : prev_vehicle_on_ride;
        Vehicle* vEDI = getGameState().entities.GetEntity<Vehicle>(vehicleIdx);
        if (vEDI == nullptr)
        {
            return UpdateMiniGolfSubroutineStatus::stop;
        }
        if (!(vEDI->mini_golf_flags & MiniGolfFlag::Flag1) || (vEDI->mini_golf_flags & MiniGolfFlag::Flag2))
        {
            remaining_distance -= 0x368A;
            if (remaining_distance < 0)
            {
                remaining_distance = 0;
            }

            if (remaining_distance < 0x368A)
            {
                Loc6DCDE4(curRide);
                return UpdateMiniGolfSubroutineStatus::stop;
            }
            acceleration = Geometry::getAccelerationFromPitch(pitch);
            _vehicleUnkF64E10++;
            return UpdateMiniGolfSubroutineStatus::restart;
        }
        if (vEDI->var_D3 != 0)
        {
            remaining_distance -= 0x368A;
            if (remaining_distance < 0)
            {
                remaining_distance = 0;
            }

            if (remaining_distance < 0x368A)
            {
                Loc6DCDE4(curRide);
                return UpdateMiniGolfSubroutineStatus::stop;
            }
            acceleration = Geometry::getAccelerationFromPitch(pitch);
            _vehicleUnkF64E10++;
            return UpdateMiniGolfSubroutineStatus::restart;
        }
        vEDI->mini_golf_flags &= ~MiniGolfFlag::Flag1;
        mini_golf_flags &= ~MiniGolfFlag::Flag1;
    }

    if (mini_golf_flags & MiniGolfFlag::Flag3)
    {
        Vehicle* vEDI = this;

        for (;;)
        {
            vEDI = getGameState().entities.GetEntity<Vehicle>(vEDI->prev_vehicle_on_ride);
            if (vEDI == this || vEDI == nullptr)
            {
                break;
            }
            if (vEDI->IsHead())
                continue;
            if (!(vEDI->mini_golf_flags & MiniGolfFlag::Flag4))
                continue;
            if (vEDI->TrackLocation != TrackLocation)
                continue;
            remaining_distance -= 0x368A;
            if (remaining_distance < 0)
            {
                remaining_distance = 0;
            }

            if (remaining_distance < 0x368A)
            {
                Loc6DCDE4(curRide);
                return UpdateMiniGolfSubroutineStatus::stop;
            }
            acceleration = Geometry::getAccelerationFromPitch(pitch);
            _vehicleUnkF64E10++;
            return UpdateMiniGolfSubroutineStatus::restart;
        }

        mini_golf_flags |= MiniGolfFlag::Flag4;
        mini_golf_flags &= ~MiniGolfFlag::Flag3;
    }

    return UpdateMiniGolfSubroutineStatus::carryOn;
}

[[nodiscard]] Vehicle::UpdateMiniGolfSubroutineStatus Vehicle::Loc6DC462(const Ride& curRide)
{
    while (true)
    {
        UpdateMiniGolfSubroutineStatus flagsStatus = UpdateMiniGolfSubroutineStatus::restart;
        while (flagsStatus == UpdateMiniGolfSubroutineStatus::restart)
        {
            flagsStatus = UpdateTrackMotionMiniGolfFlagsStatus(curRide);
        }
        if (flagsStatus == UpdateMiniGolfSubroutineStatus::stop)
            return UpdateMiniGolfSubroutineStatus::stop;

        uint16_t trackTotalProgress = GetTrackProgress();
        if (track_progress + 1 >= trackTotalProgress)
        {
            CoordsXYZ trackPos;
            Direction direction;
            auto* tileElement = MapGetTrackElementAtOfTypeSeq(TrackLocation, GetTrackType(), 0);
            CoordsXYE output;
            int32_t outZ{};
            int32_t outDirection{};
            CoordsXYE input = { TrackLocation, tileElement };
            if (!TrackBlockGetNext(&input, &output, &outZ, &outDirection))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                _vehicleVelocityF64E0C -= remaining_distance + 1;
                remaining_distance = -1;
                acceleration += Geometry::getAccelerationFromPitch(pitch);
                _vehicleUnkF64E10++;
                return UpdateMiniGolfSubroutineStatus::carryOn;
            }
            tileElement = output.element;
            trackPos = { output.x, output.y, outZ };
            direction = outDirection;

            if (PitchAndRollStart(false, tileElement) != TrackPitchAndRollEnd(GetTrackType()))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                _vehicleVelocityF64E0C -= remaining_distance + 1;
                remaining_distance = -1;
                if (remaining_distance >= 0)
                {
                    Loc6DCDE4(curRide);
                }
                acceleration += Geometry::getAccelerationFromPitch(pitch);
                _vehicleUnkF64E10++;
                return UpdateMiniGolfSubroutineStatus::carryOn;
            }

            TrackLocation = trackPos;

            if (!IsHead())
            {
                Vehicle* prevVehicle = getGameState().entities.GetEntity<Vehicle>(prev_vehicle_on_ride);
                if (prevVehicle != nullptr)
                {
                    TrackSubposition = prevVehicle->TrackSubposition;
                }
                if (TrackSubposition != VehicleTrackSubposition::MiniGolfStart9)
                {
                    TrackSubposition = VehicleTrackSubposition{ static_cast<uint8_t>(
                        static_cast<uint8_t>(TrackSubposition) - 1u) };
                }
            }

            ClearFlag(VehicleFlags::OnLiftHill);
            SetTrackType(tileElement->AsTrack()->GetTrackType());
            SetTrackDirection(direction);
            brake_speed = tileElement->AsTrack()->GetBrakeBoosterSpeed();
            track_progress = 0;
        }
        else
        {
            track_progress += 1;
        }

        if (!IsHead())
        {
            animation_frame++;
            if (animation_frame >= 6)
            {
                animation_frame = 0;
            }
        }
        const VehicleInfo* moveInfo;
        for (;;)
        {
            moveInfo = GetMoveInfo();
            if (moveInfo->x != kLocationNull)
            {
                break;
            }
            switch (MiniGolfState(moveInfo->y))
            {
                case MiniGolfState::Unk0: // Loc6DC7B4
                    if (!IsHead())
                    {
                        mini_golf_flags |= MiniGolfFlag::Flag3;
                    }
                    else
                    {
                        uint16_t rand16 = ScenarioRand() & 0xFFFF;
                        VehicleTrackSubposition nextTrackSubposition = VehicleTrackSubposition::MiniGolfBallPathC14;
                        if (rand16 <= 0xA000)
                        {
                            nextTrackSubposition = VehicleTrackSubposition::MiniGolfBallPathB12;
                            if (rand16 <= 0x900)
                            {
                                nextTrackSubposition = VehicleTrackSubposition::MiniGolfBallPathA10;
                            }
                        }
                        TrackSubposition = nextTrackSubposition;
                    }
                    track_progress++;
                    break;
                case MiniGolfState::Unk1: // Loc6DC7ED
                    LOG_ERROR("Unused move info...");
                    assert(false);
                    var_D3 = static_cast<uint8_t>(moveInfo->z);
                    track_progress++;
                    break;
                case MiniGolfState::Unk2: // Loc6DC800
                    mini_golf_flags |= MiniGolfFlag::Flag0;
                    track_progress++;
                    break;
                case MiniGolfState::Unk3: // Loc6DC810
                    mini_golf_flags |= MiniGolfFlag::Flag1;
                    track_progress++;
                    break;
                case MiniGolfState::Unk4: // Loc6DC820
                {
                    auto animation = MiniGolfAnimation(moveInfo->z);
                    // When the ride is closed occasionally the peep is removed
                    // but the vehicle is still on the track. This will prevent
                    // it from crashing in that situation.
                    auto* curPeep = getGameState().entities.TryGetEntity<Guest>(peep[0]);
                    if (curPeep != nullptr)
                    {
                        if (animation == MiniGolfAnimation::SwingLeft)
                        {
                            if (curPeep->PeepId & 7)
                            {
                                animation = MiniGolfAnimation::Swing;
                            }
                        }
                        if (animation == MiniGolfAnimation::PuttLeft)
                        {
                            if (curPeep->PeepId & 7)
                            {
                                animation = MiniGolfAnimation::Putt;
                            }
                        }
                    }
                    mini_golf_current_animation = animation;
                    animation_frame = 0;
                    track_progress++;
                    break;
                }
                case MiniGolfState::Unk5: // Loc6DC87A
                    mini_golf_flags |= MiniGolfFlag::Flag2;
                    track_progress++;
                    break;
                case MiniGolfState::Unk6: // Loc6DC88A
                    mini_golf_flags &= ~MiniGolfFlag::Flag4;
                    mini_golf_flags |= MiniGolfFlag::Flag5;
                    track_progress++;
                    break;
                default:
                    LOG_ERROR("Invalid move info...");
                    assert(false);
                    break;
            }
        }

        // Loc6DC8A1
        CoordsXYZ trackPos = { TrackLocation.x + moveInfo->x, TrackLocation.y + moveInfo->y,
                               TrackLocation.z + moveInfo->z + GetRideTypeDescriptor(curRide.type).Heights.VehicleZOffset };

        remaining_distance -= 0x368A;
        if (remaining_distance < 0)
        {
            remaining_distance = 0;
        }

        _vehicleCurPosition = trackPos;
        Orientation = moveInfo->yaw;
        roll = moveInfo->roll;
        pitch = moveInfo->pitch;

        if (this == _vehicleFrontVehicle)
        {
            if (_vehicleVelocityF64E08 >= 0)
            {
                auto otherVehicleIndex = prev_vehicle_on_ride;
                UpdateMotionCollisionDetection(trackPos, &otherVehicleIndex);
            }
        }
        if (remaining_distance < 0x368A)
        {
            Loc6DCDE4(curRide);
            return UpdateMiniGolfSubroutineStatus::stop;
        }
        acceleration = Geometry::getAccelerationFromPitch(pitch);
        _vehicleUnkF64E10++;
    }
}

[[nodiscard]] Vehicle::UpdateMiniGolfSubroutineStatus Vehicle::Loc6DCA9A(const Ride& curRide)
{
    while (true)
    {
        if (track_progress == 0)
        {
            auto tileElement = MapGetTrackElementAtOfTypeSeq(TrackLocation, GetTrackType(), 0);
            TrackBeginEnd trackBeginEnd;
            if (!TrackBlockGetPrevious({ TrackLocation, tileElement }, &trackBeginEnd))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                _vehicleVelocityF64E0C -= remaining_distance + 1;
                remaining_distance = -1;
                acceleration += Geometry::getAccelerationFromPitch(pitch);
                _vehicleUnkF64E10++;
                continue;
            }
            CoordsXYZ trackPos = { trackBeginEnd.begin_x, trackBeginEnd.begin_y, trackBeginEnd.begin_z };
            auto direction = trackBeginEnd.begin_direction;
            tileElement = trackBeginEnd.begin_element;

            if (PitchAndRollStart(false, tileElement) != TrackPitchAndRollEnd(GetTrackType()))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                _vehicleVelocityF64E0C -= remaining_distance - 0x368A;
                remaining_distance = 0x368A;
                acceleration = Geometry::getAccelerationFromPitch(pitch);
                _vehicleUnkF64E10++;
                return UpdateMiniGolfSubroutineStatus::restart;
            }

            TrackLocation = trackPos;

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

            SetTrackType(tileElement->AsTrack()->GetTrackType());
            SetTrackDirection(direction);
            brake_speed = tileElement->AsTrack()->GetBrakeBoosterSpeed();

            // There are two bytes before the move info list
            track_progress = GetTrackProgress();
        }
        else
        {
            track_progress -= 1;
        }

        auto moveInfo = GetMoveInfo();
        CoordsXYZ trackPos = { TrackLocation.x + moveInfo->x, TrackLocation.y + moveInfo->y,
                               TrackLocation.z + moveInfo->z + GetRideTypeDescriptor(curRide.type).Heights.VehicleZOffset };

        remaining_distance -= 0x368A;
        if (remaining_distance < 0)
        {
            remaining_distance = 0;
        }

        _vehicleCurPosition = trackPos;
        Orientation = moveInfo->yaw;
        roll = moveInfo->roll;
        pitch = moveInfo->pitch;

        if (this == _vehicleFrontVehicle)
        {
            if (_vehicleVelocityF64E08 >= 0)
            {
                auto otherVehicleIndex = EntityId::FromUnderlying(var_44); // Possibly wrong?.
                if (UpdateMotionCollisionDetection(trackPos, &otherVehicleIndex))
                {
                    _vehicleVelocityF64E0C -= remaining_distance - 0x368A;
                    remaining_distance = 0x368A;
                    {
                        Vehicle* vEBP = getGameState().entities.GetEntity<Vehicle>(otherVehicleIndex);
                        if (vEBP == nullptr)
                        {
                            return UpdateMiniGolfSubroutineStatus::stop;
                        }
                        Vehicle* vEDI = gCurrentVehicle;
                        if (abs(vEDI->velocity - vEBP->velocity) > 14.0_mph)
                        {
                            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION;
                        }
                        vEDI->velocity = vEBP->velocity >> 1;
                        vEBP->velocity = vEDI->velocity >> 1;
                    }
                    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_2;
                    acceleration = Geometry::getAccelerationFromPitch(pitch);
                    _vehicleUnkF64E10++;
                    return UpdateMiniGolfSubroutineStatus::restart;
                }
            }
        }

        if (remaining_distance >= 0)
        {
            Loc6DCDE4(curRide);
            return UpdateMiniGolfSubroutineStatus::stop;
        }
        acceleration += Geometry::getAccelerationFromPitch(pitch);
        _vehicleUnkF64E10++;
    }
}

/**
 *  rct2: 0x006DC3A7
 *
 *
 */

void Vehicle::UpdateTrackMotionMiniGolfVehicle(const Ride& curRide, const RideObjectEntry& rideEntry, const CarEntry* carEntry)
{
    _vehicleUnkF64E10 = 1;
    acceleration = Geometry::getAccelerationFromPitch(pitch);
    if (!HasFlag(VehicleFlags::MoveSingleCar))
    {
        remaining_distance = _vehicleVelocityF64E0C + remaining_distance;
    }
    if (remaining_distance >= 0 && remaining_distance < 0x368A)
    {
        Loc6DCE02(curRide);
        return;
    }
    sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
    _vehicleCurPosition.x = x;
    _vehicleCurPosition.y = y;
    _vehicleCurPosition.z = z;
    Invalidate();
    bool skipLoc6DC462 = (remaining_distance < 0);

    while (true)
    {
        if (!skipLoc6DC462)
        {
            auto result1 = Loc6DC462(curRide);
            if (result1 == UpdateMiniGolfSubroutineStatus::stop)
                return;
        }
        skipLoc6DC462 = false;
        auto result2 = Loc6DCA9A(curRide);
        if (result2 == UpdateMiniGolfSubroutineStatus::stop)
            return;
    }
}

void Vehicle::Loc6DCDE4(const Ride& curRide)
{
    MoveTo(_vehicleCurPosition);

    Loc6DCE02(curRide);
}

void Vehicle::Loc6DCE02(const Ride& curRide)
{
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
    if (trackType != TrackElemType::endStation)
    {
        return;
    }
    if (this != gCurrentVehicle)
    {
        return;
    }
    if (_vehicleVelocityF64E08 < 0)
    {
        if (track_progress > 11)
        {
            return;
        }
    }
    if (track_progress <= 8)
    {
        return;
    }

    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;

    for (const auto& station : curRide.getStations())
    {
        if (TrackLocation != station.Start)
        {
            continue;
        }
        if (TrackLocation.z != station.GetBaseZ())
        {
            continue;
        }
        _vehicleStationIndex = curRide.getStationIndex(&station);
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

int32_t Vehicle::UpdateTrackMotionMiniGolfCalculateAcceleration(const CarEntry& carEntry)
{
    int32_t sumAcceleration = 0;
    int32_t numVehicles = 0;
    uint16_t totalMass = 0;

    for (Vehicle* vehicle = this; vehicle != nullptr;
         vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        numVehicles++;
        totalMass += vehicle->mass;
        sumAcceleration += vehicle->acceleration;
    }

    int32_t newAcceleration = ((sumAcceleration / numVehicles) * 21) >> 9;
    newAcceleration -= velocity >> 12;
    newAcceleration -= GetAccelerationDecrease2(velocity, totalMass);

    {
        int32_t poweredAcceleration = speed << 14;
        int32_t quarterForce = (speed * totalMass) >> 2;
        if (HasFlag(VehicleFlags::PoweredCarInReverse))
        {
            poweredAcceleration = -poweredAcceleration;
        }
        poweredAcceleration -= velocity;
        poweredAcceleration *= powered_acceleration << 1;
        if (quarterForce != 0)
            poweredAcceleration /= quarterForce;

        if (abs(velocity) > 1.0_mph)
        {
            newAcceleration = 0;
        }
        newAcceleration += poweredAcceleration;
    }

    return newAcceleration;
}

int32_t Vehicle::UpdateTrackMotionMiniGolf(int32_t* outStation)
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return 0;

    const auto* rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
        return 0;

    const auto* carEntry = Entry();

    gCurrentVehicle = this;
    _vehicleMotionTrackFlags = 0;
    velocity += acceleration;
    _vehicleVelocityF64E08 = velocity;
    _vehicleVelocityF64E0C = (velocity >> 10) * 42;
    _vehicleFrontVehicle = _vehicleVelocityF64E08 < 0 ? TrainTail() : this;

    for (Vehicle* vehicle = _vehicleFrontVehicle; vehicle != nullptr;)
    {
        vehicle->UpdateTrackMotionMiniGolfVehicle(*curRide, *rideEntry, carEntry);
        if (vehicle->HasFlag(VehicleFlags::OnLiftHill))
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL;
        }
        if (vehicle->HasFlag(VehicleFlags::MoveSingleCar))
        {
            if (outStation != nullptr)
                *outStation = _vehicleStationIndex.ToUnderlying();
            return _vehicleMotionTrackFlags;
        }
        if (_vehicleVelocityF64E08 >= 0)
        {
            vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train);
        }
        else
        {
            if (vehicle == gCurrentVehicle)
            {
                break;
            }
            vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
        }
    }

    acceleration = UpdateTrackMotionMiniGolfCalculateAcceleration(*carEntry);

    if (outStation != nullptr)
        *outStation = _vehicleStationIndex.ToUnderlying();
    return _vehicleMotionTrackFlags;
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

    if (rideEntry->flags & RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND_SLIDE)
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
    if (!GetRideTypeDescriptor(curRide->type).HasFlag(RtdFlag::supportsLevelCrossings))
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

void Vehicle::Claxon() const
{
    const auto* rideEntry = GetRideEntry();
    switch (rideEntry->Cars[vehicle_type].soundRange)
    {
        case SoundRange::steamWhistle:
            Play3D(SoundId::trainWhistle, { x, y, z });
            break;
        case SoundRange::tramBell:
            Play3D(SoundId::tram, { x, y, z });
            break;
        default:
            break;
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
