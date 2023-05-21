/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "meta/Condor.h"

#include "../../common.h"
#include "../../core/DataSerialiser.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../scenario/Scenario.h"
#include "../Ride.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
#include "../VehiclePaint.h"
#include "../RideBoundboxBuilder.h"
#include "../../util/Util.h"
#include <cmath>

#include <algorithm>
#include <cstring>

enum
{
    SPR_ROTO_DROP_TOWER_SEGMENT = 14558,
    SPR_ROTO_DROP_TOWER_SEGMENT_TOP = 14559,
    SPR_ROTO_DROP_TOWER_BASE = 14560,
    SPR_ROTO_DROP_TOWER_BASE_SEGMENT = 14561,
    SPR_ROTO_DROP_TOWER_BASE_90_DEG = 14562,
    SPR_ROTO_DROP_TOWER_BASE_SEGMENT_90_DEG = 14563,
};

/**
 *
 *  rct2: 0x006D5DA9
 */
void VehicleVisualCondor(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry)
{
    //dont draw here, it doesn't work since the sprites are wider than 64 pixels!
}

static constexpr const std::array<CoordsXY, 4> StartLocations = { CoordsXY{ 0, 96 }, CoordsXY{ 96, 192 },
                                                                  CoordsXY{ 192, 96 }, CoordsXY{ 128, 0 } };

static constexpr int CondorRadius = 48;
static constexpr int CondorCenter = 96;
static constexpr float CondorAngleDelta = 0.09817f;

static constexpr int NumArmSprites = 16;

static std::array<CoordsXY, NumArmSprites*4> CalculateLocations()
{
    std::array<CoordsXY, NumArmSprites*4> res;
    int i = 0;
    for (i = 0; i < NumArmSprites*4; i++)
    {
        res[i] = CoordsXY{ static_cast<int>(CondorCenter + CondorRadius * cos(i * CondorAngleDelta)),
                           static_cast<int>(CondorCenter + CondorRadius * sin(i * CondorAngleDelta)) };
    }
    return res;
}
static auto CondorLocations = CalculateLocations();

static constexpr int NumArmSpritesSymmetry = 4;
static constexpr int NumVehicleAngles = 8;
static constexpr int NumCarsPerVehicle = 7;
static constexpr int NumCarsTiltAngles = 8;

// degrees/second
static constexpr float MaxVehicleRotationSpeed = 120;
static constexpr float MinVehicleRotationSpeed = 10;
static constexpr float MaxTowerRotationSpeed = 30;
static constexpr float MinTowerRotationSpeed = 10;

//rise time
static constexpr int32_t MaxRiseFrameTime = 4;
static constexpr int32_t MinRiseFrameTime = 2;

//standing at the top of the tower
constexpr int SpinningTopTime = 120;

static int GetTowerRotationFrameTime(int currentHeight, int minHeight, int maxHeight)
{
    float progress = static_cast<float>(currentHeight - minHeight) / static_cast<float>(maxHeight - minHeight);

    //spin faster at the middle point
    float rotationSpeed = CubicLerp(
        progress, -1.0f, 0.0f, 0.5f, 1.0f, MinTowerRotationSpeed, MinTowerRotationSpeed, MaxTowerRotationSpeed,
        MinTowerRotationSpeed);

    float rotationFrameTime = 1.0f / rotationSpeed / static_cast<float>(NumArmSprites) * NumArmSpritesSymmetry * 360.0f;
    return static_cast<int>(round(rotationFrameTime));
}

static int GetVehicleRotationFrameTime(int currentHeight, int minHeight, int maxHeight)
{
    float progress = static_cast<float>(currentHeight - minHeight) / static_cast<float>(maxHeight - minHeight);

    //the max vehicle rotation speed is at the top
    float rotationSpeed = CubicLerp(
        progress, -1.0f, 0.0f, 1.0f, 2.0f, MinVehicleRotationSpeed, MinVehicleRotationSpeed, MaxVehicleRotationSpeed,
        MaxVehicleRotationSpeed);

    float rotationFrameTime = 1.0f / rotationSpeed / static_cast<float>(NumVehicleAngles) * static_cast<float>(NumCarsPerVehicle) * 360.0f;
    return static_cast<int>(round(rotationFrameTime));
}

static int GetCondorRiseFrameTime(int currentHeight, int minHeight, int maxHeight)
{
    float progress = static_cast<float>(currentHeight - minHeight) / static_cast<float>(maxHeight - minHeight);

    //maximum raise speed is at middle of tower
    auto riseFrameTime = CubicLerp(
        progress, -1.0f, 0.0f, 0.5f, 1.0f, MaxRiseFrameTime, MaxRiseFrameTime, MinRiseFrameTime, MaxRiseFrameTime);
    return static_cast<int>(round(riseFrameTime));
}

static int MaxVehicleRotationFrameTime = GetVehicleRotationFrameTime(0, 0, 100);
static int MinVehicleRotationFrameTime = GetVehicleRotationFrameTime(100, 0, 100);

// tilt angle is function of the vehicle roation speed
constexpr int NumTiltAngles = 8;
static uint8_t GetVehicleTilt(int vehicleFrameTime)
{
    float progress = static_cast<float>(vehicleFrameTime - MinVehicleRotationFrameTime)
        / static_cast<float>(MaxVehicleRotationFrameTime - MinVehicleRotationFrameTime);
    return Lerp(NumCarsTiltAngles - 1, 0, progress);
}

static void PaintCondorStructure(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, CoordsXYZ bbOffset, CoordsXYZ bbSize)
{
    //dont paint the vehicles if the ride is closed
    if (ride.status == RideStatus::Closed)
        return;

    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && !ride.vehicles[0].IsNull())
    {
        session.InteractionType = ViewportInteractionItem::Entity;
        session.CurrentlyDrawnEntity = GetEntity<Vehicle>(ride.vehicles[0]);
    }

    auto imageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
    auto imageFlags = session.TrackColours[SCHEME_MISC];
    if (imageFlags != TrackGhost)
    {
        imageTemplate = imageFlags;
    }
    

    std::array<CoordsXY, 4> offsets = StartLocations;
    std::array<CoordsXY, 4>
            offsets2 = { CoordsXY{ 50, 0 }, CoordsXY{ 0, -50 }, CoordsXY{ -50, 0 }, CoordsXY{ 0, 50 } };

    auto condorRideData = static_cast<CondorRideData*>(ride.Data.get());
    if (condorRideData != nullptr)
    {
        auto vehicleZ = condorRideData->VehiclesZ;
        auto tilt = GetVehicleTilt(condorRideData->VehicleRotationFrameTime);
        auto armsImageId = imageTemplate.WithIndex(
            rideEntry->Cars[0].base_image_id + NumVehicleAngles * NumTiltAngles + (condorRideData->ArmRotation % 16));
        for (int i = 0; i < 4; i++)
        {
            int locationIndex = i * NumArmSprites;
            locationIndex += condorRideData->ArmRotation;
            locationIndex %= (NumArmSprites * 4);

            auto imageId = imageTemplate.WithIndex(
                rideEntry->Cars[0].base_image_id + tilt * NumVehicleAngles + condorRideData->QuadRotation[i]);
            PaintAddImageAsParent(
                session, imageId,
                { -bbOffset.x + CondorLocations[locationIndex].x, -bbOffset.y + CondorLocations[locationIndex].y,
                  32 + vehicleZ },
                { { CondorLocations[locationIndex].x, CondorLocations[locationIndex].y, vehicleZ + 32 }, { 24, 24, 12 } });
            PaintAddImageAsChild(
                session, armsImageId, { -bbOffset.x + CondorCenter, -bbOffset.y + CondorCenter, vehicleZ + 32 },
                { { CondorCenter, CondorCenter, 32 + vehicleZ }, { 24, 24, 12 } });
        }
    }

}

/** rct2: 0x00886194 */
static void PaintCondorBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = TrackMap9x9[direction][trackSequence];

    int32_t edges = Edges9x9[trackSequence];

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    const StationObject* stationObject = ride.GetStationObject();

    TrackPaintUtilPaintFloor(session, edges, session.TrackColours[SCHEME_SUPPORTS], height, floorSpritesMetalB, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, session.TrackColours[SCHEME_TRACK], height, fenceSpritesMetalB,
        session.CurrentRotation);

    if (trackSequence == 24)
    {
        auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction & 1 ? SPR_ROTO_DROP_TOWER_BASE_90_DEG : SPR_ROTO_DROP_TOWER_BASE));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 27 }, { 8, 8, height + 3 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction & 1 ? SPR_ROTO_DROP_TOWER_BASE_SEGMENT_90_DEG : SPR_ROTO_DROP_TOWER_BASE_SEGMENT));
        PaintAddImageAsParent(session, imageId, { 0, 0, height + 32 }, { 2, 2, 30 }, { 8, 8, height + 32 });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
            (direction & 1 ? SPR_ROTO_DROP_TOWER_BASE_SEGMENT_90_DEG : SPR_ROTO_DROP_TOWER_BASE_SEGMENT));
        PaintAddImageAsParent(session, imageId, { 0, 0, height + 64 }, { 2, 2, 30 }, { 8, 8, height + 64 });

        PaintUtilSetVerticalTunnel(session, height + 96);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);

        PaintUtilSetGeneralSupportHeight(session, height + 96, 0x20);

        return;
    }

    const auto& bbBuilder = RideBoundboxBuilder(TowerBase9x9TED.Block, 49);
    auto offsets = bbBuilder.GetOffsets();

    auto sequenceValid = std::find_if(
        offsets.begin(), offsets.end(), [trackSequence](const auto& elem) { return elem.first == trackSequence; });
    if (sequenceValid != offsets.end())
        PaintCondorStructure(
            session, ride, trackSequence, direction, height, trackElement, CoordsXYZ{ sequenceValid->second, 32 },
            CoordsXYZ{ 32, 32, 32 });

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 1:
            blockedSegments = SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 2:
            blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 3:
            blockedSegments = SEGMENT_B4 | SEGMENT_CC | SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 4:
            blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8;
            break;
        case 5:
            blockedSegments = SEGMENT_BC | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 6:
            blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0;
            break;
        case 7:
            blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC;
            break;
        case 8:
            blockedSegments = SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0;
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, blockedSegments, 0xFFFF, 0);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL & ~blockedSegments, height + 2, 0x20);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008861A4 */
static void PaintCondorTowerSection(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence == 1)
    {
        return;
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_ROTO_DROP_TOWER_SEGMENT);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 2, 30 }, { 8, 8, height });

    const TileElement* nextTileElement = reinterpret_cast<const TileElement*>(&trackElement) + 1;
    if (trackElement.IsLastForTile() || trackElement.GetClearanceZ() != nextTileElement->GetBaseZ())
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_ROTO_DROP_TOWER_SEGMENT_TOP);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 2, 2, 30 } });
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);

    PaintUtilSetVerticalTunnel(session, height + 32);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/**
 * rct2: 0x00886074
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionCondor(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::FlatTrack7x7:
            return PaintCondorBase;

        case TrackElemType::TowerSection:
            return PaintCondorTowerSection;
    }

    return nullptr;
}

static constexpr const CoordsXY word_9A3AB4[4] = {
    { 0, 0 },
    { 0, -96 },
    { -96, -96 },
    { -96, 0 },
};

CondorVehicleData::CondorVehicleData()
    : VehicleIndex(0)
{
}

CondorVehicleData ::~CondorVehicleData()
{
}

VehicleDataType CondorVehicleData::GetType() const
{
    return VehicleDataType::Condor;
}

void CondorVehicleData::Serialise(DataSerialiser& stream)
{
    stream << VehicleIndex;
}

RideDataType CondorRideData::GetType() const
{
    return RideDataType::Condor;
}

CondorRideData::CondorRideData()
    : State(CondorRideState::Waiting)
    , VehiclesZ(0)
    , TowerTop(0)
    , TowerBase(0)
    , ArmRotation(0)
    , TowerRotationCounter(0)
    , SpinningTopCounter(0)
    , VehicleRotationCounter(0)
    , RiseFrameCounter(0)
    , RiseFrameTime(MaxRiseFrameTime)
{
    QuadRotation = { 0, 0, 0, 0 };
    InitialQuadRotation = QuadRotation[0];
    TowerRotationFrameTime = GetTowerRotationFrameTime(0, 0, 100);
    VehicleRotationFrameTime = GetVehicleRotationFrameTime(0, 0, 100);
}

static uint32_t CondorGetTowerHeight(const Vehicle& vehicle)
{
    TileCoordsXYZ result;

    TileElement* tileElement = MapGetTrackElementAtOfType(vehicle.TrackLocation, vehicle.GetTrackType());
    if (tileElement == nullptr)
        return 0;

    while (!tileElement->IsLastForTile())
    {
        tileElement++;

        if (tileElement->IsGhost())
            continue;

        if (tileElement->GetType() != TileElementType::Track)
            continue;

        const auto* trackElement = tileElement->AsTrack();
        if (trackElement->GetRideIndex() != vehicle.ride)
            continue;

        if (trackElement->GetTrackType() != TrackElemType::TowerSection
            && trackElement->GetTrackType() != TrackElemType::TowerBase)
            continue;

        int32_t tileTop = tileElement->ClearanceHeight;
        if (result.z < tileTop)
            result.z = tileTop;
    }

    return result.ToCoordsXYZ().z;
}

void CondorCreateVehicle(
    Vehicle* vehicle, Ride* ride, int32_t carIndex, const CoordsXYZ& carPosition, TrackElement* trackElement)
{
    // Loc6DDCA4:
    vehicle->TrackSubposition = VehicleTrackSubposition::Default;
    vehicle->TrackLocation = carPosition;
    vehicle->current_station = trackElement->GetStationIndex();

    vehicle->SetTrackDirection(0);
    vehicle->SetTrackType(trackElement->GetTrackType());
    vehicle->track_progress = 0;
    vehicle->SetState(Vehicle::Status::MovingToEndOfStation);
    vehicle->update_flags = 0;

    // place the car in a circle, centered around the tower
    auto centerOffset = CoordsXY{ CondorCenter, CondorCenter };
    auto chosenLoc = carPosition + CoordsXYZ{ StartLocations[carIndex], 0 };
    vehicle->MoveTo(chosenLoc);
    vehicle->sprite_direction = 0;
    vehicle->Pitch = 0;
    vehicle->TrackLocation = carPosition + CoordsXYZ{ centerOffset, 0 };

    vehicle->VehicleData = std::make_unique<CondorVehicleData>();
    auto condorData = static_cast<CondorVehicleData*>(vehicle->VehicleData.get());
    condorData->VehicleIndex = carIndex;

    if (carIndex == 0)
    {
        ride->Data = std::make_unique<CondorRideData>();

        auto condorRideData = static_cast<CondorRideData*>(ride->Data.get());
        condorRideData->State = CondorRideState::Waiting;
        condorRideData->TowerTop = CondorGetTowerHeight(*vehicle) - 64;
        condorRideData->TowerBase = vehicle->z;
        condorRideData->VehiclesZ = vehicle->z;
    }
}

void CondorRideUpdateWating(Ride& ride)
{
    auto condorRideData = static_cast<CondorRideData*>(ride.Data.get());

    // check if all vehicles are departing
    const auto trainId = ride.vehicles[0];
    auto* car = GetEntity<Vehicle>(trainId);

    if (car != nullptr)
    {
        if (car->status == Vehicle::Status::Travelling)
        {
            // car->FinishDeparting();
            condorRideData->State = CondorRideState::Climbing;
        }
    }
}

static void UpdateRotation(CondorRideData* condorRideData)
{
    condorRideData->TowerRotationFrameTime = GetTowerRotationFrameTime(condorRideData->VehiclesZ, condorRideData->TowerBase, condorRideData->TowerTop);
    condorRideData->TowerRotationCounter++;

    if (condorRideData->TowerRotationCounter >= condorRideData->TowerRotationFrameTime)
    {
        condorRideData->ArmRotation++;
        condorRideData->ArmRotation %= (NumArmSprites * 4);
        condorRideData->TowerRotationCounter = 0;
    }

    condorRideData->VehicleRotationFrameTime = GetVehicleRotationFrameTime(
        condorRideData->VehiclesZ, condorRideData->TowerBase, condorRideData->TowerTop);
    condorRideData->VehicleRotationCounter++;

    if (condorRideData->VehicleRotationCounter >= condorRideData->VehicleRotationFrameTime)
    {
        for (auto& quadRot : condorRideData->QuadRotation)
        {
            quadRot++;
            quadRot %= 8;
        }
        condorRideData->VehicleRotationCounter = 0;
    }
}

static void CondorRideUpdateClimbing(Ride& ride)
{
    auto condorRideData = static_cast<CondorRideData*>(ride.Data.get());
    if (condorRideData != nullptr)
    {
        int32_t height = condorRideData->VehiclesZ;
        condorRideData->RiseFrameTime = GetCondorRiseFrameTime(
            condorRideData->VehiclesZ, condorRideData->TowerBase, condorRideData->TowerTop);
        condorRideData->RiseFrameCounter++;
        if (condorRideData->RiseFrameCounter >= condorRideData->RiseFrameTime)
        {
            condorRideData->RiseFrameCounter = 0;
            height++;
        }

        if (height > condorRideData->TowerTop)
        {
            height = condorRideData->TowerTop;
            condorRideData->State = CondorRideState::SpinningAtTop;
        }
        condorRideData->VehiclesZ = height;

        UpdateRotation(condorRideData);
    }
}

static void CondorRideUpdateFalling(Ride& ride)
{
    auto condorRideData = static_cast<CondorRideData*>(ride.Data.get());
    if (condorRideData != nullptr)
    {
        condorRideData->RiseFrameTime = GetCondorRiseFrameTime(
            condorRideData->VehiclesZ, condorRideData->TowerBase, condorRideData->TowerTop);
        condorRideData->RiseFrameCounter++;
        int32_t height = condorRideData->VehiclesZ;
        if (condorRideData->RiseFrameCounter >= condorRideData->RiseFrameTime)
        {
            condorRideData->RiseFrameCounter = 0;
            height--;
        }

        auto oldArmRotation = condorRideData->ArmRotation;
        auto oldQuadRotation = condorRideData->QuadRotation[0];
        UpdateRotation(condorRideData);

        if (height < condorRideData->TowerBase)
        {
            height = condorRideData->TowerBase;

            if (oldArmRotation % (NumArmSprites * 4) == 0)
                condorRideData->ArmRotation = 0;

            if (oldQuadRotation == condorRideData->InitialQuadRotation)
            {
                condorRideData->QuadRotation[0] = condorRideData->InitialQuadRotation;
                condorRideData->QuadRotation[1] = condorRideData->QuadRotation[0] + NumArmSprites;
                condorRideData->QuadRotation[2] = condorRideData->QuadRotation[1] + NumArmSprites;
                condorRideData->QuadRotation[3] = condorRideData->QuadRotation[2] + NumArmSprites;
            }
                

            if (condorRideData->ArmRotation != 0)
                condorRideData->State = CondorRideState::Falling;
            else if (condorRideData->QuadRotation[0] != condorRideData->InitialQuadRotation)
                condorRideData->State = CondorRideState::Falling;
            else
            {
                condorRideData->State = CondorRideState::Waiting;
            }
                

        }
        condorRideData->VehiclesZ = height;
    }
}


static void CondorRideUpdateSpinningAtTop(Ride& ride)
{
    auto condorRideData = static_cast<CondorRideData*>(ride.Data.get());
    if (condorRideData != nullptr)
    {
        UpdateRotation(condorRideData);

        //dont spin the arms when at top
        condorRideData->TowerRotationCounter = 0;

        condorRideData->SpinningTopCounter++;
        if (condorRideData->SpinningTopCounter >= SpinningTopTime)
        {
            condorRideData->SpinningTopCounter = 0;
            condorRideData->State = CondorRideState::Falling;
        }
            
    }
}

void CondorRideUpdate(Ride& ride)
{
    auto condorRideData = static_cast<CondorRideData*>(ride.Data.get());
    if (condorRideData != nullptr)
    {
        switch (condorRideData->State)
        {
            case CondorRideState::Waiting:
                CondorRideUpdateWating(ride);
                break;
            case CondorRideState::Climbing:
                CondorRideUpdateClimbing(ride);
                break;
            case CondorRideState::Falling:
                CondorRideUpdateFalling(ride);
                break;
            case CondorRideState::SpinningAtTop:
                CondorRideUpdateSpinningAtTop(ride);
                break;
        }
    }
}

void CondorUpdateDeparting(Vehicle& vehicle)
{
    vehicle.FinishDeparting();
}

void CondorUpdateTravelling(Vehicle& vehicle)
{
    // auto condorData = static_cast<CondorVehicleData*>(vehicle.VehicleData.get());
    auto ride = vehicle.GetRide();
    auto condorRideData = static_cast<CondorRideData*>(ride->Data.get());

    if (condorRideData != nullptr)
    {
        auto* car = &vehicle;
        do
        {
            if (condorRideData->State == CondorRideState::Climbing || condorRideData->State == CondorRideState::Falling
                || condorRideData->State == CondorRideState::SpinningAtTop)
            {
                auto target = car->GetLocation();
                target.z = condorRideData->VehiclesZ;

                car->MoveTo(target);
                car->TrackLocation.z = target.z;
                car->Invalidate();
            }
            else
            {
                car->SetState(Vehicle::Status::Arriving);
            }
            car = GetEntity<Vehicle>(car->next_vehicle_on_train);
        } while (car != nullptr);
    }
}

void CondorUpdateMotion(Vehicle& vehicle)
{
    vehicle.current_station = StationIndex::FromUnderlying(0);
    vehicle.velocity = 0;
    vehicle.acceleration = 0;
    vehicle.SetState(Vehicle::Status::WaitingForPassengers);
}

void CondorUpdateWaitingForDepart(Vehicle& vehicle)
{
}

void CondorRideData::Reset()
{
    State = CondorRideState::Waiting;
    VehiclesZ = TowerBase;
    ArmRotation = 0;
    TowerRotationCounter = 0;
    SpinningTopCounter = 0;
    VehicleRotationCounter = 0;
    RiseFrameCounter = 0;
    RiseFrameTime = MaxRiseFrameTime;
    QuadRotation = { 0, 0, 0, 0 };
    InitialQuadRotation = QuadRotation[0];
    TowerRotationFrameTime = GetTowerRotationFrameTime(0, 0, 100);
    VehicleRotationFrameTime = GetVehicleRotationFrameTime(0, 0, 100);
}
