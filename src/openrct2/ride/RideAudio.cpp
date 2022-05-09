/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../object/AudioObject.h"
#include "../object/MusicObject.h"
#include "../object/ObjectManager.h"
#include "Ride.h"

#include <algorithm>
#include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

namespace OpenRCT2::RideAudio
{
    constexpr uint8_t TUNE_ID_NULL = 0xFF;
    constexpr size_t MAX_RIDE_MUSIC_CHANNELS = 32;

    /**
     * Represents a particular instance of ride music that can be heard in a viewport.
     * These are created each frame via enumerating each ride / viewport.
     */
    struct ViewportRideMusicInstance
    {
        ::RideId RideId;
        uint8_t TrackIndex{};

        size_t Offset{};
        int16_t Volume{};
        int16_t Pan{};
        uint16_t Frequency{};
    };

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

        void* Channel{};

        RideMusicChannel(const ViewportRideMusicInstance& instance, void* channel)
        {
            RideId = instance.RideId;
            TrackIndex = instance.TrackIndex;

            Offset = std::max<size_t>(0, instance.Offset - 10000);
            Volume = instance.Volume;
            Pan = instance.Pan;
            Frequency = instance.Frequency;

            Channel = channel;

            Mixer_Channel_SetOffset(channel, Offset);
            Mixer_Channel_Volume(channel, DStoMixerVolume(Volume));
            Mixer_Channel_Pan(channel, DStoMixerPan(Pan));
            Mixer_Channel_Rate(channel, DStoMixerRate(Frequency));
        }

        RideMusicChannel(const RideMusicChannel&) = delete;

        RideMusicChannel(RideMusicChannel&& src) noexcept
        {
            *this = std::move(src);
        }

        RideMusicChannel& operator=(RideMusicChannel&& src) noexcept
        {
            RideId = src.RideId;
            TrackIndex = src.TrackIndex;

            Offset = src.Offset;
            Volume = src.Volume;
            Pan = src.Pan;
            Frequency = src.Frequency;

            if (Channel != nullptr)
            {
                Mixer_Stop_Channel(Channel);
            }
            Channel = src.Channel;
            src.Channel = nullptr;

            return *this;
        }

        ~RideMusicChannel()
        {
            if (Channel != nullptr)
            {
                Mixer_Stop_Channel(Channel);
                Channel = nullptr;
            }
        }

        bool IsPlaying() const
        {
            if (Channel != nullptr)
            {
                return Mixer_Channel_IsPlaying(Channel);
            }
            return false;
        }

        size_t GetOffset() const
        {
            if (Channel != nullptr)
            {
                return Mixer_Channel_GetOffset(Channel);
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
                    Mixer_Channel_Volume(Channel, DStoMixerVolume(Volume));
                }
            }
            if (Pan != instance.Pan)
            {
                Pan = instance.Pan;
                if (Channel != nullptr)
                {
                    Mixer_Channel_Pan(Channel, DStoMixerPan(Pan));
                }
            }
            if (Frequency != instance.Frequency)
            {
                Frequency = instance.Frequency;
                if (Channel != nullptr)
                {
                    Mixer_Channel_Rate(Channel, DStoMixerRate(Frequency));
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

    static void StartRideMusicChannel(const ViewportRideMusicInstance& instance)
    {
        auto& objManager = GetContext()->GetObjectManager();

        // Create new music channel
        auto ride = get_ride(instance.RideId);
        if (ride->type == RIDE_TYPE_CIRCUS)
        {
            ObjectEntryDescriptor desc(ObjectType::Audio, AudioObjectIdentifiers::Rct2Circus);
            auto audioObj = static_cast<AudioObject*>(objManager.GetLoadedObject(desc));
            if (audioObj != nullptr)
            {
                auto source = audioObj->GetSample(0);
                if (source != nullptr)
                {
                    auto channel = Mixer_Play_Music(source, MIXER_LOOP_NONE, true);
                    if (channel != nullptr)
                    {
                        // Move circus music to the sound mixer group
                        channel->SetGroup(Audio::MixerGroup::Sound);
                        _musicChannels.emplace_back(instance, channel);
                    }
                }
            }
        }
        else
        {
            auto musicObj = static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, ride->music));
            if (musicObj != nullptr)
            {
                auto track = musicObj->GetTrack(instance.TrackIndex);
                if (track != nullptr)
                {
                    auto stream = track->Asset.GetStream();
                    auto channel = Mixer_Play_Music(std::move(stream), MIXER_LOOP_NONE);
                    if (channel != nullptr)
                    {
                        _musicChannels.emplace_back(instance, channel);
                    }
                }
            }
        }
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
        if (gGameSoundsOff || !gConfigSound.ride_music_enabled)
            return;

        StopInactiveRideMusicChannels();
        for (const auto& instance : _musicInstances)
        {
            UpdateRideMusicChannelForMusicParams(instance);
        }
    }

    static std::pair<size_t, size_t> RideMusicGetTrackOffsetLength(const Ride& ride)
    {
        if (ride.type == RIDE_TYPE_CIRCUS)
        {
            return { 1378, 12427456 };
        }

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
            auto rotatedCoords = translate_3d_to_2d_with_z(get_current_rotation(), rideCoords);
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
                auto screenWidth = std::max(context_get_width(), 64);
                auto panX = ((x2 / screenWidth) - 0x8000) >> 4;

                auto y2 = (viewport->pos.y + viewport->zoom.ApplyInversedTo(rotatedCoords.y - viewport->viewPos.y)) * 0x10000;
                auto screenHeight = std::max(context_get_height(), 64);
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
