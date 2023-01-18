/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideAudio.h"

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../audio/AudioChannel.h"
#include "../audio/AudioContext.h"
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../object/AudioObject.h"
#include "../object/MusicObject.h"
#include "../object/ObjectManager.h"
#include "Ride.h"
#include "RideData.h"

#include <algorithm>
#include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

namespace OpenRCT2::RideAudio
{
    constexpr size_t MAX_RIDE_MUSIC_CHANNELS = 32;

    /**
     * Represents an audio channel to play a particular ride's music track.
     */
    struct RideMusicChannel
    {
        ::RideId RideId{};
        uint8_t TrackIndex{};

        size_t Offset{};
        int16_t Volume{};
        int16_t Pan{};
        uint16_t Frequency{};

        std::shared_ptr<IAudioChannel> Channel{};
        IAudioSource* Source{};

        RideMusicChannel(
            const ViewportRideMusicInstance& instance, std::shared_ptr<IAudioChannel> channel, IAudioSource* source)
        {
            RideId = instance.RideId;
            TrackIndex = instance.TrackIndex;

            Offset = std::max<size_t>(0, instance.Offset - 10000);
            Volume = instance.Volume;
            Pan = instance.Pan;
            Frequency = instance.Frequency;

            channel->SetOffset(Offset);
            channel->SetVolume(DStoMixerVolume(Volume));
            channel->SetPan(DStoMixerPan(Pan));
            channel->SetRate(DStoMixerRate(Frequency));
            Channel = std::move(channel);

            Source = source;
        }

        RideMusicChannel(const RideMusicChannel&) = delete;

        RideMusicChannel(RideMusicChannel&& src) noexcept
        {
            *this = std::move(src);
        }

        RideMusicChannel& operator=(RideMusicChannel&& src) noexcept
        {
            using std::swap;

            RideId = src.RideId;
            TrackIndex = src.TrackIndex;

            Offset = src.Offset;
            Volume = src.Volume;
            Pan = src.Pan;
            Frequency = src.Frequency;

            swap(Channel, src.Channel);
            swap(Source, src.Source);

            return *this;
        }

        ~RideMusicChannel()
        {
            if (Channel != nullptr)
            {
                Channel->Stop();
            }
            if (Source != nullptr)
            {
                Source->Release();
            }
        }

        bool IsPlaying() const
        {
            if (Channel != nullptr)
            {
                return Channel->IsPlaying();
            }
            return false;
        }

        size_t GetOffset() const
        {
            if (Channel != nullptr)
            {
                return Channel->GetOffset();
            }
            return 0;
        }

        void Update(const ViewportRideMusicInstance& instance)
        {
            if (Volume != instance.Volume)
            {
                Volume = instance.Volume;
                if (Channel != nullptr)
                {
                    Channel->SetVolume(DStoMixerVolume(Volume));
                }
            }
            if (Pan != instance.Pan)
            {
                Pan = instance.Pan;
                if (Channel != nullptr)
                {
                    Channel->SetPan(DStoMixerPan(Pan));
                }
            }
            if (Frequency != instance.Frequency)
            {
                Frequency = instance.Frequency;
                if (Channel != nullptr)
                {
                    Channel->SetRate(DStoMixerRate(Frequency));
                }
            }
        }
    };

    static std::vector<ViewportRideMusicInstance> _musicInstances;
    static std::vector<RideMusicChannel> _musicChannels;

    void StopAllChannels()
    {
        _musicChannels.clear();
    }

    void ClearAllViewportInstances()
    {
        _musicInstances.clear();
    }

    void DefaultStartRideMusicChannel(const ViewportRideMusicInstance& instance)
    {
        auto& objManager = GetContext()->GetObjectManager();
        auto ride = GetRide(instance.RideId);
        auto musicObj = static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, ride->music));
        if (musicObj != nullptr)
        {
            auto shouldLoop = musicObj->GetTrackCount() == 1;
            auto source = musicObj->GetTrackSample(instance.TrackIndex);
            if (source != nullptr)
            {
                auto channel = CreateAudioChannel(source, MixerGroup::RideMusic, shouldLoop, 0);
                if (channel != nullptr)
                {
                    _musicChannels.emplace_back(instance, channel, source);
                }
            }
        }
    }
    void CircusStartRideMusicChannel(const ViewportRideMusicInstance& instance)
    {
        auto& objManager = GetContext()->GetObjectManager();
        ObjectEntryDescriptor desc(ObjectType::Audio, AudioObjectIdentifiers::Rct2Circus);
        auto audioObj = static_cast<AudioObject*>(objManager.GetLoadedObject(desc));
        if (audioObj != nullptr)
        {
            auto source = audioObj->GetSample(0);
            if (source != nullptr)
            {
                auto channel = CreateAudioChannel(source, MixerGroup::Sound, false, 0);
                if (channel != nullptr)
                {
                    _musicChannels.emplace_back(instance, channel, nullptr);
                }
            }
        }
    }

    static void StartRideMusicChannel(const ViewportRideMusicInstance& instance)
    {
        // Create new music channel
        auto ride = GetRide(instance.RideId);
        const auto& rtd = ride->GetRideTypeDescriptor();
        rtd.StartRideMusic(instance);
    }

    static void StopInactiveRideMusicChannels()
    {
        _musicChannels.erase(
            std::remove_if(
                _musicChannels.begin(), _musicChannels.end(),
                [](const auto& channel) {
                    auto found = std::any_of(_musicInstances.begin(), _musicInstances.end(), [&channel](const auto& instance) {
                        return instance.RideId == channel.RideId && instance.TrackIndex == channel.TrackIndex;
                    });
                    if (!found || !channel.IsPlaying())
                    {
                        return true;
                    }

                    return false;
                }),
            _musicChannels.end());
    }

    static void UpdateRideMusicChannelForMusicParams(const ViewportRideMusicInstance& instance)
    {
        // Find existing music channel
        auto foundChannel = std::find_if(
            _musicChannels.begin(), _musicChannels.end(), [&instance](const RideMusicChannel& channel) {
                return channel.RideId == instance.RideId && channel.TrackIndex == instance.TrackIndex;
            });

        if (foundChannel != _musicChannels.end())
        {
            foundChannel->Update(instance);
        }
        else if (_musicChannels.size() < MAX_RIDE_MUSIC_CHANNELS)
        {
            StartRideMusicChannel(instance);
        }
    }

    /**
     * Start, update and stop audio channels for each ride music instance that can be heard across all viewports.
     */
    void UpdateMusicChannels()
    {
        if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) != 0 || (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) != 0)
            return;

        // TODO Allow circus music (CSS24) to play if ride music is disabled (that should be sound)
        if (gGameSoundsOff || !gConfigSound.RideMusicEnabled)
            return;

        StopInactiveRideMusicChannels();
        for (const auto& instance : _musicInstances)
        {
            UpdateRideMusicChannelForMusicParams(instance);
        }
    }

    std::pair<size_t, size_t> RideMusicGetTrackOffsetLength_Circus(const Ride& ride)
    {
        return { 1378, 12427456 };
    }

    std::pair<size_t, size_t> RideMusicGetTrackOffsetLength_Default(const Ride& ride)
    {
        auto& objManager = GetContext()->GetObjectManager();
        auto musicObj = static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, ride.music));
        if (musicObj != nullptr)
        {
            auto numTracks = musicObj->GetTrackCount();
            if (ride.music_tune_id < numTracks)
            {
                auto track = musicObj->GetTrack(ride.music_tune_id);
                return { track->BytesPerTick, track->Size };
            }
        }
        return { 0, 0 };
    }

    static std::pair<size_t, size_t> RideMusicGetTrackOffsetLength(const Ride& ride)
    {
        const auto& rtd = ride.GetRideTypeDescriptor();
        return rtd.MusicTrackOffsetLength(ride);
    }

    static void RideUpdateMusicPosition(Ride& ride)
    {
        auto [trackOffset, trackLength] = RideMusicGetTrackOffsetLength(ride);
        auto position = ride.music_position + trackOffset;
        if (position < trackLength)
        {
            ride.music_position = position;
        }
        else
        {
            ride.music_tune_id = TUNE_ID_NULL;
            ride.music_position = 0;
        }
    }

    static void RideUpdateMusicPosition(
        Ride& ride, size_t offset, size_t length, int16_t volume, int16_t pan, uint16_t sampleRate)
    {
        if (offset < length)
        {
            if (_musicInstances.size() < MAX_RIDE_MUSIC_CHANNELS)
            {
                auto& instance = _musicInstances.emplace_back();
                instance.RideId = ride.id;
                instance.TrackIndex = ride.music_tune_id;
                instance.Offset = offset;
                instance.Volume = volume;
                instance.Pan = pan;
                instance.Frequency = sampleRate;
            }
            ride.music_position = static_cast<uint32_t>(offset);
        }
        else
        {
            ride.music_tune_id = TUNE_ID_NULL;
            ride.music_position = 0;
        }
    }

    static void RideUpdateMusicPosition(Ride& ride, int16_t volume, int16_t pan, uint16_t sampleRate)
    {
        auto foundChannel = std::find_if(_musicChannels.begin(), _musicChannels.end(), [&ride](const auto& channel) {
            return channel.RideId == ride.id && channel.TrackIndex == ride.music_tune_id;
        });

        auto [trackOffset, trackLength] = RideMusicGetTrackOffsetLength(ride);
        if (foundChannel != _musicChannels.end())
        {
            if (foundChannel->IsPlaying())
            {
                // Since we have a real music channel, use the offset from that
                auto newOffset = foundChannel->GetOffset();
                RideUpdateMusicPosition(ride, newOffset, trackLength, volume, pan, sampleRate);
            }
            else
            {
                // We had a real music channel, but it isn't playing anymore, so stop the track
                ride.music_position = 0;
                ride.music_tune_id = TUNE_ID_NULL;
            }
        }
        else
        {
            // We do not have a real music channel, so simulate the playing of the music track
            auto newOffset = ride.music_position + trackOffset;
            RideUpdateMusicPosition(ride, newOffset, trackLength, volume, pan, sampleRate);
        }
    }

    static uint8_t CalculateVolume(int32_t pan)
    {
        uint8_t result = 255;
        int32_t v = std::min(std::abs(pan), 6143) - 2048;
        if (v > 0)
        {
            v = -((v / 4) - 1024) / 4;
            result = static_cast<uint8_t>(std::clamp(v, 0, 255));
        }
        return result;
    }

    /**
     * Register an instance of audible ride music for this frame at the given coordinates.
     */
    void UpdateMusicInstance(Ride& ride, const CoordsXYZ& rideCoords, uint16_t sampleRate)
    {
        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gGameSoundsOff && g_music_tracking_viewport != nullptr)
        {
            auto rotatedCoords = Translate3DTo2DWithZ(GetCurrentRotation(), rideCoords);
            auto viewport = g_music_tracking_viewport;
            auto viewWidth = viewport->view_width;
            auto viewWidth2 = viewWidth * 2;
            auto viewX = viewport->viewPos.x - viewWidth2;
            auto viewY = viewport->viewPos.y - viewWidth;
            auto viewX2 = viewWidth2 + viewWidth2 + viewport->view_width + viewX;
            auto viewY2 = viewWidth + viewWidth + viewport->view_height + viewY;
            if (viewX >= rotatedCoords.x || viewY >= rotatedCoords.y || viewX2 < rotatedCoords.x || viewY2 < rotatedCoords.y)
            {
                RideUpdateMusicPosition(ride);
            }
            else
            {
                auto x2 = (viewport->pos.x + viewport->zoom.ApplyInversedTo(rotatedCoords.x - viewport->viewPos.x)) * 0x10000;
                auto screenWidth = std::max(ContextGetWidth(), 64);
                auto panX = ((x2 / screenWidth) - 0x8000) >> 4;

                auto y2 = (viewport->pos.y + viewport->zoom.ApplyInversedTo(rotatedCoords.y - viewport->viewPos.y)) * 0x10000;
                auto screenHeight = std::max(ContextGetHeight(), 64);
                auto panY = ((y2 / screenHeight) - 0x8000) >> 4;

                auto volX = CalculateVolume(panX);
                auto volY = CalculateVolume(panY);
                auto volXY = std::min(volX, volY);
                if (volXY < gVolumeAdjustZoom * 3)
                {
                    volXY = 0;
                }
                else
                {
                    volXY = volXY - (gVolumeAdjustZoom * 3);
                }

                int16_t newVolume = -((static_cast<uint8_t>(-volXY - 1) * static_cast<uint8_t>(-volXY - 1)) / 16) - 700;
                if (volXY != 0 && newVolume >= -4000)
                {
                    auto newPan = std::clamp(panX, -10000, 10000);
                    RideUpdateMusicPosition(ride, newVolume, newPan, sampleRate);
                }
                else
                {
                    RideUpdateMusicPosition(ride);
                }
            }
        }
    }
} // namespace OpenRCT2::RideAudio
