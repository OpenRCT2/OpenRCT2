/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../ride/RideTypes.h"
#include "AudioMixer.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

struct CoordsXYZ;

namespace OpenRCT2::Audio
{
    constexpr size_t kMaxVehicleSounds = 14;
    constexpr uint16_t kSoundIdNull = 0xFFFF;
    constexpr int32_t kAudioPlayAtCentre = 0x8000;

    struct IAudioChannel;
    struct IAudioSource;
    enum class MixerGroup : int32_t;
    enum class SoundId : uint8_t;

    struct Sound
    {
        SoundId id;
        int16_t volume;
        int16_t pan;
        uint16_t frequency;
        std::shared_ptr<IAudioChannel> channel;
    };

    struct VehicleSound
    {
        uint16_t id;
        int16_t volume;
        Sound trackSound;
        Sound otherSound;
    };

    struct VehicleSoundParams
    {
        uint16_t id;
        int16_t panX;
        int16_t panY;
        uint16_t frequency;
        int16_t volume;
        uint16_t priority;
    };

    enum class SoundId : uint8_t
    {
        liftClassic,
        trackFrictionClassicWood,
        frictionClassic,
        scream1,
        click1,
        click2,
        placeItem,
        scream2,
        scream3,
        scream4,
        scream5,
        scream6,
        liftFrictionWheels,
        purchase,
        crash,
        layingOutWater,
        water1,
        water2,
        trainWhistle,
        trainDeparting,
        waterSplash,
        goKartEngine,
        rideLaunch1,
        rideLaunch2,
        cough1,
        cough2,
        cough3,
        cough4,
        rain,
        thunder1,
        thunder2,
        trackFrictionTrain,
        trackFrictionWater,
        balloonPop,
        mechanicFix,
        scream7,
        toiletFlush,
        click3,
        quack,
        newsItem,
        windowOpen,
        laugh1,
        laugh2,
        laugh3,
        applause,
        hauntedHouseScare,
        hauntedHouseScream1,
        hauntedHouseScream2,
        blockBrakeClose,
        blockBrakeRelease,
        error,
        brakeRelease,
        liftArrow,
        liftWood,
        trackFrictionWood,
        liftWildMouse,
        liftBM,
        trackFrictionBM,
        scream8, // Blood-curdling, Haunted House-like scream
        tram,
        doorOpen,
        doorClose,
        portcullis,
        crowdAmbience,
        liftRMC,
        trackFrictionRMC,
        liftFlume,
        noScream = 254,
        null = 255
    };

    constexpr uint8_t kRCT2SoundCount = static_cast<uint32_t>(SoundId::portcullis) + 1;

    namespace AudioObjectIdentifiers
    {
        constexpr std::string_view kRCT1Title = "rct1.audio.title";
        // virtual name, used by either RCT2Base or RCTCBase, depending on which one is loaded.
        constexpr std::string_view kRCT2 = "rct2.audio.base";
        constexpr std::string_view kRCT2Base = "rct2.audio.base.rct2";
        constexpr std::string_view kRCTCBase = "rct2.audio.base.rctc";
        constexpr std::string_view kRCT2Title = "rct2.audio.title";
        constexpr std::string_view kOpenRCT2Title = "openrct2.audio.title";
        constexpr std::string_view kRCT2Circus = "rct2.audio.circus";
        constexpr std::string_view kOpenRCT2Additional = "openrct2.audio.additional";
    } // namespace AudioObjectIdentifiers

    extern bool gGameSoundsOff;
    extern int32_t gVolumeAdjustZoom;

    extern VehicleSound gVehicleSoundList[kMaxVehicleSounds];

    enum class DoorSoundType : uint8_t
    {
        none,
        door,
        portcullis,
    };
    constexpr uint8_t kDoorSoundTypeCount = 3;

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
     * Stops all sound effects.
     */
    void StopSFX();

    /**
     * Stops all sound.
     */
    void StopAll();

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
     * Temporarily stops playing sounds until Resume() is called.
     * rct2: 0x006BABB4
     */
    void Pause();

    /**
     * Plays the specified sound.
     * @param soundId The sound effect to play.
     * @param volume The volume at which the sound effect should be played.
     * @param pan The pan at which the sound effect should be played. If set to anything other than kAudioPlayAtCentre, plays
     * the sound at a position relative to the centre of the viewport.
     */
    void Play(SoundId soundId, int32_t volume, int32_t pan);

    /**
     * Plays the specified sound at a virtual location.
     * @param soundId The sound effect to play.
     * @param loc The coordinates of the location.
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
     * Resumes playing sounds that had been paused by a call to Pause().
     * rct2: 0x006BABD8
     */
    void Resume();

    std::shared_ptr<IAudioChannel> CreateAudioChannel(
        SoundId soundId, bool loop = false, int32_t volume = kMixerVolumeMax, float pan = 0.5f, double rate = 1,
        bool forget = false);
    std::shared_ptr<IAudioChannel> CreateAudioChannel(
        IAudioSource* source, MixerGroup group, bool loop = false, int32_t volume = kMixerVolumeMax, float pan = 0.5f,
        double rate = 1, bool forget = false);

    int32_t DStoMixerVolume(int32_t volume);
    float DStoMixerPan(int32_t pan);
    double DStoMixerRate(int32_t frequency);

} // namespace OpenRCT2::Audio
