/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../common.h"
#include "../ride/RideTypes.h"
#include "AudioMixer.h"

#include <vector>

class AudioObject;
struct CoordsXYZ;

namespace OpenRCT2::Audio
{
    constexpr size_t MaxDeviceNameSize = 256;
    constexpr size_t MaxVehicleSounds = 14;
    constexpr size_t MaxDefaultMusic = 46;
    constexpr uint16_t SoundIdNull = 0xFFFF;

#define AUDIO_PLAY_AT_CENTRE 0x8000

    struct IAudioChannel;
    struct IAudioSource;
    enum class MixerGroup : int32_t;
    enum class SoundId : uint8_t;

    struct Sound
    {
        SoundId Id;
        int16_t Volume;
        int16_t Pan;
        uint16_t Frequency;
        std::shared_ptr<IAudioChannel> Channel;
    };

    struct VehicleSound
    {
        uint16_t id;
        int16_t volume;
        Sound TrackSound;
        Sound OtherSound;
    };

    struct VehicleSoundParams
    {
        uint16_t id;
        int16_t pan_x;
        int16_t pan_y;
        uint16_t frequency;
        int16_t volume;
        uint16_t priority;
    };

    enum class SoundId : uint8_t
    {
        LiftClassic,
        TrackFrictionClassicWood,
        FrictionClassic,
        Scream1,
        Click1,
        Click2,
        PlaceItem,
        Scream2,
        Scream3,
        Scream4,
        Scream5,
        Scream6,
        LiftFrictionWheels,
        Purchase,
        Crash,
        LayingOutWater,
        Water1,
        Water2,
        TrainWhistle,
        TrainDeparting,
        WaterSplash,
        GoKartEngine,
        RideLaunch1,
        RideLaunch2,
        Cough1,
        Cough2,
        Cough3,
        Cough4,
        Rain,
        Thunder1,
        Thunder2,
        TrackFrictionTrain,
        TrackFrictionWater,
        BalloonPop,
        MechanicFix,
        Scream7,
        ToiletFlush,
        Click3,
        Quack,
        NewsItem,
        WindowOpen,
        Laugh1,
        Laugh2,
        Laugh3,
        Applause,
        HauntedHouseScare,
        HauntedHouseScream1,
        HauntedHouseScream2,
        BlockBrakeClose,
        BlockBrakeRelease,
        Error,
        BrakeRelease,
        LiftArrow,
        LiftWood,
        TrackFrictionWood,
        LiftWildMouse,
        LiftBM,
        TrackFrictionBM,
        Scream8,
        Tram,
        DoorOpen,
        DoorClose,
        Portcullis,
        CrowdAmbience,
        NoScream = 254,
        Null = 255
    };

    constexpr uint8_t RCT2SoundCount = static_cast<uint32_t>(SoundId::Portcullis) + 1;

    namespace AudioObjectIdentifiers
    {
        constexpr std::string_view Rct1Title = "rct1.audio.title";
        constexpr std::string_view Rct2Base = "rct2.audio.base";
        constexpr std::string_view Rct2Title = "rct2.audio.title";
        constexpr std::string_view Rct2Circus = "rct2.audio.circus";
    } // namespace AudioObjectIdentifiers

    extern bool gGameSoundsOff;
    extern int32_t gVolumeAdjustZoom;

    extern VehicleSound gVehicleSoundList[MaxVehicleSounds];

    /**
     * Returns false when no audio device is available or when audio is turned off, otherwise true.
     */
    bool IsAvailable();

    /*
     * Returns the amount of available audio devices.
     */
    int32_t GetDeviceCount();

    /**
     * Returns the device name by index.
     */
    const std::string& GetDeviceName(int32_t index);

    /**
     * Returns the currently used device index, -1 if not available.
     */
    int32_t GetCurrentDeviceIndex();

    /**
     * Deregisters the audio device.
     * rct2: 0x006BAB21
     */
    void Close();

    /**
     * Initialises the audio subsystem.
     */
    void Init();

    void LoadAudioObjects();

    /**
     * Loads the ride sounds and info.
     * rct2: 0x006BA8E0
     */
    void InitRideSoundsAndInfo();

    /**
     * Loads the ride sounds.
     * rct2: 0x006BA9B5
     */
    void InitRideSounds(int32_t device);

    /**
     * Temporarily stops playing sounds until audio_unpause_sounds() is called.
     * rct2: 0x006BABB4
     */
    void Pause();

    /**
     * Plays the specified sound.
     * @param soundId The sound effect to play.
     * @param volume The volume at which the sound effect should be played.
     * @param pan The pan at which the sound effect should be played. If set to anything other than AUDIO_PLAY_AT_CENTRE, plays
     * the sound at a position relative to the centre of the viewport.
     */
    void Play(SoundId soundId, int32_t volume, int32_t pan);

    /**
     * Plays the specified sound at a virtual location.
     * @param soundId The sound effect to play.
     * @param x The x coordinate of the location.
     * @param y The y coordinate of the location.
     * @param z The z coordinate of the location.
     */
    void Play3D(SoundId soundId, const CoordsXYZ& loc);

    /**
     * Populates the gAudioDevices array with the available audio devices.
     */
    void PopulateDevices();

    /**
     * Starts playing the title music.
     * rct2: 0x006BD0F8
     */
    void PlayTitleMusic();

    /**
     * Stops the title music from playing.
     * rct2: 0x006BD0BD
     */
    void StopTitleMusic();

    /**
     * Stops vehicle sounds from playing.
     * rct2: 0x006BABDF
     */
    void StopVehicleSounds();

    /**
     * Toggles whether all sounds should be played.
     * rct2: 0x006BAB8A
     */
    void ToggleAllSounds();

    /**
     * Resumes playing sounds that had been paused by a call to audio_pause_sounds().
     * rct2: 0x006BABD8
     */
    void Resume();

    void StopAll();

    AudioObject* GetBaseAudioObject();

    std::shared_ptr<IAudioChannel> CreateAudioChannel(
        SoundId soundId, bool loop = false, int32_t volume = MIXER_VOLUME_MAX, float pan = 0.5f, double rate = 1,
        bool forget = false);
    std::shared_ptr<IAudioChannel> CreateAudioChannel(
        IAudioSource* source, MixerGroup group, bool loop = false, int32_t volume = MIXER_VOLUME_MAX, float pan = 0.5f,
        double rate = 1, bool forget = false);

    int32_t DStoMixerVolume(int32_t volume);
    float DStoMixerPan(int32_t pan);
    double DStoMixerRate(int32_t frequency);

} // namespace OpenRCT2::Audio
