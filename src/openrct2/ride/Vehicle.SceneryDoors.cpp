/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Vehicle.h"

#include "../audio/Audio.h"
#include "../core/EnumUtils.hpp"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "../world/tile_element/WallElement.h"
#include "Ride.h"
#include "RideData.h"
#include "TrackData.h"
#include "ted/TrackElementDescriptor.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::TrackMetadata;

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
    const auto& trackBlock = ted.sequenceData.sequences[ted.sequenceData.numSequences - 1].clearance;
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
 *  rct2: 0x006DEDE8
 */
void Vehicle::UpdateSceneryDoorBackwards() const
{
    auto trackType = GetTrackType();
    const auto& ted = GetTrackElementDescriptor(trackType);
    const TrackCoordinates* trackCoordinates = &ted.coordinates;
    auto wallCoords = CoordsXYZ{ TrackLocation,
                                 TrackLocation.z - ted.sequenceData.sequences[0].clearance.z + trackCoordinates->zBegin };
    int32_t direction = (GetTrackDirection() + trackCoordinates->rotationBegin) & 3;
    direction = DirectionReverse(direction);

    AnimateSceneryDoor<true>({ wallCoords, static_cast<Direction>(direction) }, TrackLocation, next_vehicle_on_train.IsNull());
}
