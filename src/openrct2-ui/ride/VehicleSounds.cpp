#include "VehicleSounds.h"

#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../windows/Windows.h"

#include <cassert>
#include <numeric>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/audio/AudioChannel.h>
#include <openrct2/audio/AudioMixer.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/profiling/Profiling.h>
#include <openrct2/ride/TrainManager.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/tile_element/SurfaceElement.h>
#include <sfl/static_vector.hpp>

namespace OpenRCT2::Audio
{
    namespace
    {
        template<typename T>
        class TrainIterator;
        template<typename T>
        class Train
        {
        public:
            explicit Train(T* vehicle)
                : FirstCar(vehicle)
            {
                assert(FirstCar->IsHead());
            }
            int32_t GetMass() const;

            friend class TrainIterator<T>;
            using iterator = TrainIterator<T>;
            iterator begin() const
            {
                return iterator{ FirstCar };
            }
            iterator end() const
            {
                return iterator{};
            }

        private:
            T* FirstCar;
        };
        template<typename T>
        class TrainIterator
        {
        public:
            using iterator = TrainIterator;
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using pointer = T*;
            using reference = T&;

            TrainIterator() = default;
            explicit TrainIterator(T* vehicle)
                : Current(vehicle)
            {
            }
            reference operator*() const
            {
                return *Current;
            }
            iterator& operator++()
            {
                Current = getGameState().entities.GetEntity<Vehicle>(NextVehicleId);
                if (Current != nullptr)
                {
                    NextVehicleId = Current->next_vehicle_on_train;
                }
                return *this;
            }
            iterator operator++(int)
            {
                iterator temp = *this;
                ++*this;
                return temp;
            }
            bool operator!=(const iterator& other) const
            {
                return Current != other.Current;
            }

        private:
            T* Current = nullptr;
            EntityId NextVehicleId = EntityId::GetNull();
        };
    } // namespace

    template<typename T>
    int32_t Train<T>::GetMass() const
    {
        return std::accumulate(
            begin(), end(), 0, [](int32_t totalMass, const Vehicle& vehicle) { return totalMass + vehicle.mass; });
    }

    static bool SoundCanPlay(const Vehicle& vehicle)
    {
        if (gLegacyScene == LegacyScene::scenarioEditor)
            return false;

        if (gLegacyScene == LegacyScene::trackDesigner && getGameState().editorStep != EditorStep::RollercoasterDesigner)
            return false;

        if (vehicle.sound1_id == SoundId::null && vehicle.sound2_id == SoundId::null)
            return false;

        if (vehicle.x == kLocationNull)
            return false;

        if (gMusicTrackingViewport == nullptr)
            return false;

        const auto quarter_w = gMusicTrackingViewport->ViewWidth() / 4;
        const auto quarter_h = gMusicTrackingViewport->ViewHeight() / 4;

        auto left = gMusicTrackingViewport->viewPos.x;
        auto bottom = gMusicTrackingViewport->viewPos.y;

        if (Ui::Windows::WindowGetClassification(*gWindowAudioExclusive) == WindowClass::mainWindow)
        {
            left -= quarter_w;
            bottom -= quarter_h;
        }

        if (left >= vehicle.SpriteData.SpriteRect.GetRight() || bottom >= vehicle.SpriteData.SpriteRect.GetBottom())
            return false;

        auto right = gMusicTrackingViewport->ViewWidth() + left;
        auto top = gMusicTrackingViewport->ViewHeight() + bottom;

        if (Ui::Windows::WindowGetClassification(*gWindowAudioExclusive) == WindowClass::mainWindow)
        {
            right += quarter_w + quarter_w;
            top += quarter_h + quarter_h;
        }

        if (right < vehicle.SpriteData.SpriteRect.GetRight() || top < vehicle.SpriteData.SpriteRect.GetTop())
            return false;

        return true;
    }

    /**
     *
     *  rct2: 0x006BC2F3
     */
    static uint16_t GetSoundPriority(const Vehicle& vehicle)
    {
        int32_t result = Train(&vehicle).GetMass() + (std::abs(vehicle.velocity) >> 13);

        for (const auto& vehicleSound : gVehicleSoundList)
        {
            if (vehicleSound.id == vehicle.Id.ToUnderlying())
            {
                // Vehicle sounds will get higher priority if they are already playing
                return result + 300;
            }
        }

        return result;
    }

    static VehicleSoundParams CreateSoundParam(const Vehicle& vehicle, uint16_t priority)
    {
        VehicleSoundParams param;
        param.priority = priority;
        int32_t panX = (vehicle.SpriteData.SpriteRect.GetLeft() / 2) + (vehicle.SpriteData.SpriteRect.GetRight() / 2)
            - gMusicTrackingViewport->viewPos.x;
        panX = gMusicTrackingViewport->zoom.ApplyInversedTo(panX);
        panX += gMusicTrackingViewport->pos.x;

        uint16_t screenWidth = ContextGetWidth();
        if (screenWidth < 64)
        {
            screenWidth = 64;
        }
        param.panX = ((((panX * 65536) / screenWidth) - 0x8000) >> 4);

        int32_t panY = (vehicle.SpriteData.SpriteRect.GetTop() / 2) + (vehicle.SpriteData.SpriteRect.GetBottom() / 2)
            - gMusicTrackingViewport->viewPos.y;
        panY = gMusicTrackingViewport->zoom.ApplyInversedTo(panY);
        panY += gMusicTrackingViewport->pos.y;

        uint16_t screenHeight = ContextGetHeight();
        if (screenHeight < 64)
        {
            screenHeight = 64;
        }
        param.panY = ((((panY * 65536) / screenHeight) - 0x8000) >> 4);

        int32_t frequency = std::abs(vehicle.velocity);

        const auto* rideType = vehicle.GetRideEntry();
        if (rideType != nullptr)
        {
            if (rideType->Cars[vehicle.vehicle_type].double_sound_frequency & 1)
            {
                frequency *= 2;
            }
        }

        // * 0.0105133...
        frequency >>= 5; // /32
        frequency *= 5512;
        frequency >>= 14; // /16384

        frequency += 11025;
        frequency += 16 * vehicle.dopplerShift;
        param.frequency = static_cast<uint16_t>(frequency);
        param.id = vehicle.Id.ToUnderlying();
        param.volume = 0;

        if (vehicle.x != kLocationNull)
        {
            auto surfaceElement = MapGetSurfaceElementAt(CoordsXY{ vehicle.x, vehicle.y });

            // vehicle underground
            if (surfaceElement != nullptr && surfaceElement->GetBaseZ() > vehicle.z)
            {
                param.volume = 0x30;
            }
        }
        return param;
    }

    /**
     *
     *  rct2: 0x006BB9FF
     */
    static void UpdateSoundParams(
        const Vehicle& vehicle, sfl::static_vector<VehicleSoundParams, kMaxVehicleSounds>& vehicleSoundParamsList)
    {
        if (!SoundCanPlay(vehicle))
            return;

        uint16_t soundPriority = GetSoundPriority(vehicle);
        // Find a sound param of lower priority to use
        auto soundParamIter = std::find_if(
            vehicleSoundParamsList.begin(), vehicleSoundParamsList.end(),
            [soundPriority](const auto& param) { return soundPriority > param.priority; });

        if (soundParamIter == std::end(vehicleSoundParamsList))
        {
            if (vehicleSoundParamsList.size() < kMaxVehicleSounds)
            {
                vehicleSoundParamsList.push_back(CreateSoundParam(vehicle, soundPriority));
            }
        }
        else
        {
            if (vehicleSoundParamsList.size() < kMaxVehicleSounds)
            {
                // Shift all sound params down one if using a free space
                vehicleSoundParamsList.insert(soundParamIter, CreateSoundParam(vehicle, soundPriority));
            }
            else
            {
                *soundParamIter = CreateSoundParam(vehicle, soundPriority);
            }
        }
    }

    static void VehicleSoundsUpdateWindowSetup()
    {
        gMusicTrackingViewport = nullptr;

        WindowBase* window = Ui::Windows::WindowGetListening();
        if (window == nullptr)
        {
            return;
        }

        Viewport* viewport = WindowGetViewport(window);
        if (viewport == nullptr)
        {
            return;
        }

        gMusicTrackingViewport = viewport;
        gWindowAudioExclusive = window;
        if (viewport->zoom <= ZoomLevel{ 0 })
            gVolumeAdjustZoom = 0;
        else if (viewport->zoom == ZoomLevel{ 1 })
            gVolumeAdjustZoom = 35;
        else
            gVolumeAdjustZoom = 70;
    }

    static uint8_t VehicleSoundsUpdateGetPanVolume(VehicleSoundParams* sound_params)
    {
        uint8_t vol1 = 0xFF;
        uint8_t vol2 = 0xFF;

        int16_t panY = std::abs(sound_params->panY);
        panY = std::min(static_cast<int16_t>(0xFFF), panY);
        panY -= 0x800;
        if (panY > 0)
        {
            panY = (0x400 - panY) / 4;
            vol1 = LoByte(panY);
            if (static_cast<int8_t>(HiByte(panY)) != 0)
            {
                vol1 = 0xFF;
                if (static_cast<int8_t>(HiByte(panY)) < 0)
                {
                    vol1 = 0;
                }
            }
        }

        int16_t panX = std::abs(sound_params->panX);
        panX = std::min(static_cast<int16_t>(0xFFF), panX);
        panX -= 0x800;

        if (panX > 0)
        {
            panX = (0x400 - panX) / 4;
            vol2 = LoByte(panX);
            if (static_cast<int8_t>(HiByte(panX)) != 0)
            {
                vol2 = 0xFF;
                if (static_cast<int8_t>(HiByte(panX)) < 0)
                {
                    vol2 = 0;
                }
            }
        }

        vol1 = std::min(vol1, vol2);
        return std::max(0, vol1 - gVolumeAdjustZoom);
    }

    /*  Returns the vehicle sound for a sound_param.
     *
     *  If already playing returns sound.
     *  If not playing allocates a sound slot to sound_param->id.
     *  If no free slots returns nullptr.
     */
    static VehicleSound* VehicleSoundsUpdateGetVehicleSound(VehicleSoundParams* sound_params)
    {
        // Search for already playing vehicle sound
        for (auto& vehicleSound : gVehicleSoundList)
        {
            if (vehicleSound.id == sound_params->id)
                return &vehicleSound;
        }

        // No sound already playing
        for (auto& vehicleSound : gVehicleSoundList)
        {
            // Use free slot
            if (vehicleSound.id == kSoundIdNull)
            {
                vehicleSound.id = sound_params->id;
                vehicleSound.trackSound.id = SoundId::null;
                vehicleSound.otherSound.id = SoundId::null;
                vehicleSound.volume = 0x30;
                return &vehicleSound;
            }
        }

        return nullptr;
    }

    static bool IsLoopingSound(SoundId id)
    {
        switch (id)
        {
            case SoundId::liftClassic:
            case SoundId::trackFrictionClassicWood:
            case SoundId::frictionClassic:
            case SoundId::liftFrictionWheels:
            case SoundId::goKartEngine:
            case SoundId::trackFrictionTrain:
            case SoundId::trackFrictionWater:
            case SoundId::liftArrow:
            case SoundId::liftWood:
            case SoundId::trackFrictionWood:
            case SoundId::liftWildMouse:
            case SoundId::liftBM:
            case SoundId::trackFrictionBM:
            case SoundId::liftRMC:
            case SoundId::trackFrictionRMC:
            case SoundId::liftFlume:
                return true;
            default:
                return false;
        }
    }

    static bool IsFixedFrequencySound(SoundId id)
    {
        switch (id)
        {
            case SoundId::scream1:
            case SoundId::scream2:
            case SoundId::scream3:
            case SoundId::scream4:
            case SoundId::scream5:
            case SoundId::scream6:
            case SoundId::scream7:
            case SoundId::scream8:
            case SoundId::trainWhistle:
            case SoundId::trainDeparting:
            case SoundId::tram:
                return true;
            default:
                return false;
        }
    }

    static bool IsSpecialFrequencySound(SoundId id)
    {
        switch (id)
        {
            case SoundId::trackFrictionBM:
            case SoundId::trackFrictionRMC:
                return true;
            default:
                return false;
        }
    }

    enum class SoundType
    {
        TrackNoises,
        OtherNoises, // e.g. Screams
    };

    template<SoundType type>
    static uint16_t SoundFrequency(const SoundId id, uint16_t baseFrequency)
    {
        if constexpr (type == SoundType::TrackNoises)
        {
            if (IsSpecialFrequencySound(id))
            {
                return (baseFrequency / 2) + 4000;
            }
            return baseFrequency;
        }
        else
        {
            if (IsFixedFrequencySound(id))
            {
                return 22050;
            }
            return std::min((baseFrequency * 2) - 3248, 25700);
        }
    }

    template<SoundType type>
    static bool ShouldUpdateChannelRate(const SoundId id)
    {
        return type == SoundType::TrackNoises || !IsFixedFrequencySound(id);
    }

    template<SoundType type>
    static void UpdateSound(const SoundId id, int32_t volume, VehicleSoundParams* sound_params, Sound& sound, uint8_t panVol)
    {
        volume *= panVol;
        volume = volume / 8;
        volume = std::max(volume - 0x1FFF, -10000);

        if (sound.channel != nullptr && sound.channel->IsDone() && IsLoopingSound(sound.id))
        {
            sound.id = SoundId::null;
            sound.channel = nullptr;
        }
        if (id != sound.id && sound.id != SoundId::null)
        {
            sound.id = SoundId::null;
            sound.channel->Stop();
        }
        if (id == SoundId::null)
        {
            return;
        }

        if (sound.id == SoundId::null)
        {
            auto frequency = SoundFrequency<type>(id, sound_params->frequency);
            auto looping = IsLoopingSound(id);
            auto pan = sound_params->panX;
            auto channel = CreateAudioChannel(
                id, looping, DStoMixerVolume(volume), DStoMixerPan(pan), DStoMixerRate(frequency), false);
            if (channel != nullptr)
            {
                sound.id = id;
                sound.pan = sound_params->panX;
                sound.volume = volume;
                sound.frequency = sound_params->frequency;
                sound.channel = channel;
            }
            else
            {
                sound.id = SoundId::null;
            }
            return;
        }
        if (volume != sound.volume)
        {
            sound.volume = volume;
            sound.channel->SetVolume(DStoMixerVolume(volume));
        }
        if (sound_params->panX != sound.pan)
        {
            sound.pan = sound_params->panX;
            sound.channel->SetPan(DStoMixerPan(sound_params->panX));
        }
        if (!(getGameState().currentTicks & 3) && sound_params->frequency != sound.frequency)
        {
            sound.frequency = sound_params->frequency;
            if (ShouldUpdateChannelRate<type>(id))
            {
                uint16_t frequency = SoundFrequency<type>(id, sound_params->frequency);
                sound.channel->SetRate(DStoMixerRate(frequency));
            }
        }
    }

    /**
     *
     *  rct2: 0x006BBC6B
     */
    void UpdateVehicleSounds()
    {
        PROFILED_FUNCTION();

        if (!IsAvailable())
            return;

        sfl::static_vector<VehicleSoundParams, kMaxVehicleSounds> vehicleSoundParamsList;

        VehicleSoundsUpdateWindowSetup();

        for (auto vehicle : TrainManager::View())
        {
            UpdateSoundParams(*vehicle, vehicleSoundParamsList);
        }

        // Stop all playing sounds that no longer have priority to play after vehicle_update_sound_params
        for (auto& vehicleSound : gVehicleSoundList)
        {
            if (vehicleSound.id != kSoundIdNull)
            {
                bool keepPlaying = false;
                for (auto vehicleSoundParams : vehicleSoundParamsList)
                {
                    if (vehicleSound.id == vehicleSoundParams.id)
                    {
                        keepPlaying = true;
                        break;
                    }
                }

                if (keepPlaying)
                    continue;

                if (vehicleSound.trackSound.id != SoundId::null)
                {
                    vehicleSound.trackSound.channel->Stop();
                }
                if (vehicleSound.otherSound.id != SoundId::null)
                {
                    vehicleSound.otherSound.channel->Stop();
                }
                vehicleSound.id = kSoundIdNull;
            }
        }

        for (auto& vehicleSoundParams : vehicleSoundParamsList)
        {
            uint8_t panVol = VehicleSoundsUpdateGetPanVolume(&vehicleSoundParams);

            auto* vehicleSound = VehicleSoundsUpdateGetVehicleSound(&vehicleSoundParams);
            // No free vehicle sound slots (RCT2 corrupts the pointer here)
            if (vehicleSound == nullptr)
                continue;

            // Move the Sound Volume towards the SoundsParam Volume
            int32_t tempvolume = vehicleSound->volume;
            if (tempvolume != vehicleSoundParams.volume)
            {
                if (tempvolume < vehicleSoundParams.volume)
                {
                    tempvolume += 4;
                }
                else
                {
                    tempvolume -= 4;
                }
            }
            vehicleSound->volume = tempvolume;
            panVol = std::max(0, panVol - tempvolume);

            Vehicle* vehicle = getGameState().entities.GetEntity<Vehicle>(EntityId::FromUnderlying(vehicleSoundParams.id));
            if (vehicle != nullptr)
            {
                UpdateSound<SoundType::TrackNoises>(
                    vehicle->sound1_id, vehicle->sound1_volume, &vehicleSoundParams, vehicleSound->trackSound, panVol);
                UpdateSound<SoundType::OtherNoises>(
                    vehicle->sound2_id, vehicle->sound2_volume, &vehicleSoundParams, vehicleSound->otherSound, panVol);
            }
        }
    }
} // namespace OpenRCT2::Audio
