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
#include "../core/Speed.hpp"
#include "../entity/EntityRegistry.h"
#include "../scenario/Scenario.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "Ride.h"
#include "RideData.h"
#include "VehicleGeometry.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::RideVehicle;

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

void VehicleSoundsUpdate()
{
    auto windowManager = Ui::GetWindowManager();
    windowManager->BroadcastIntent(Intent(INTENT_ACTION_UPDATE_VEHICLE_SOUNDS));
}

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
