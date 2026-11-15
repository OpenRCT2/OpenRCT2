/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ManagedSounds.h"

#include "../Context.h"
#include "../interface/Viewport.h"
#include "Audio.h"
#include "AudioChannel.h"
#include "AudioSource.h"

#include <algorithm>
#include <vector>

namespace OpenRCT2::Audio::ManagedSounds
{
    struct ManagedSound
    {
        int32_t id{};
        std::shared_ptr<void> owner;
        std::shared_ptr<IAudioChannel> channel;
        IAudioSource* source{};
        MixerGroup group{};
        ManagedSoundParams params;

        // Last applied values. While paused these hold the values to resume with.
        int32_t volume{};
        float pan{};
        double rate{};
        int32_t loop{};
        uint64_t offset{};
        bool paused{};
    };

    static std::vector<ManagedSound> _sounds;
    static int32_t _nextId = 1;
    static bool _paused = false;

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

    bool GetMusicLocationParams(const CoordsXYZ& location, int16_t& volume, int16_t& pan)
    {
        auto viewport = gMusicTrackingViewport;
        if (viewport == nullptr)
            return false;

        auto rotatedCoords = Translate3DTo2DWithZ(GetCurrentRotation(), location);
        auto viewWidth = viewport->ViewWidth();
        auto viewWidth2 = viewWidth * 2;
        auto viewX = viewport->viewPos.x - viewWidth2;
        auto viewY = viewport->viewPos.y - viewWidth;
        auto viewX2 = viewWidth2 + viewWidth2 + viewport->ViewWidth() + viewX;
        auto viewY2 = viewWidth + viewWidth + viewport->ViewHeight() + viewY;
        if (viewX >= rotatedCoords.x || viewY >= rotatedCoords.y || viewX2 < rotatedCoords.x || viewY2 < rotatedCoords.y)
        {
            return false;
        }

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
        if (volXY == 0 || newVolume < -4000)
        {
            return false;
        }

        volume = newVolume;
        pan = static_cast<int16_t>(std::clamp(panX, -10000, 10000));
        return true;
    }

    static void GetVolumeAndPan(const ManagedSoundParams& params, int32_t& volume, float& pan)
    {
        if (params.isMusic)
        {
            int16_t dsVolume{};
            int16_t dsPan{};
            if (GetMusicLocationParams(params.position, dsVolume, dsPan))
            {
                volume = DStoMixerVolume(dsVolume);
                pan = DStoMixerPan(dsPan);
            }
            else
            {
                volume = 0;
                pan = DStoMixerPan(0);
            }
        }
        else
        {
            GetLocationParams(params.sampleModifier, params.position, volume, pan);
        }
    }

    static void StopChannel(ManagedSound& sound)
    {
        if (sound.channel != nullptr)
        {
            sound.channel->Stop();
        }
    }

    static void Prune()
    {
        _sounds.erase(
            std::remove_if(
                _sounds.begin(), _sounds.end(),
                [](const ManagedSound& s) { return !s.paused && (s.channel == nullptr || s.channel->IsDone()); }),
            _sounds.end());
    }

    static ManagedSound* Find(int32_t id)
    {
        auto it = std::find_if(_sounds.begin(), _sounds.end(), [id](const ManagedSound& s) { return s.id == id; });
        return it != _sounds.end() ? &*it : nullptr;
    }

    int32_t Play(IAudioSource* source, MixerGroup group, const ManagedSoundParams& params, std::shared_ptr<void> owner)
    {
        if (source == nullptr || !IsAvailable())
            return 0;

        int32_t volume = kMixerVolumeMax;
        float pan = DStoMixerPan(0);
        if (params.hasLocation)
            GetVolumeAndPan(params, volume, pan);

        auto channel = CreateAudioChannel(source, group, params.repeat != 0, volume, pan, params.rate, true);
        if (channel == nullptr)
            return 0;

        if (params.repeat > 0)
            channel->SetLoop(params.repeat);
        if (params.startOffset > 0)
            channel->SetOffset(params.startOffset);

        Prune();

        ManagedSound sound;
        sound.id = _nextId++;
        sound.owner = std::move(owner);
        sound.channel = std::move(channel);
        sound.source = source;
        sound.group = group;
        sound.params = params;
        sound.volume = volume;
        sound.pan = pan;
        sound.rate = params.rate;
        _sounds.push_back(std::move(sound));
        return _sounds.back().id;
    }

    void Stop(int32_t id)
    {
        auto* sound = Find(id);
        if (sound != nullptr)
        {
            StopChannel(*sound);
            _sounds.erase(_sounds.begin() + (sound - _sounds.data()));
        }
    }

    void StopOwner(const std::shared_ptr<void>& owner)
    {
        for (auto it = _sounds.begin(); it != _sounds.end();)
        {
            if (it->owner == owner)
            {
                StopChannel(*it);
                it = _sounds.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void StopSource(const IAudioSource* source)
    {
        for (auto it = _sounds.begin(); it != _sounds.end();)
        {
            if (it->source == source)
            {
                StopChannel(*it);
                it = _sounds.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    bool SetRate(int32_t id, double rate)
    {
        Prune();
        auto* sound = Find(id);
        if (sound == nullptr)
            return false;

        sound->rate = rate;
        if (!sound->paused)
        {
            sound->channel->SetRate(rate);
        }
        return true;
    }

    bool SetLocation(int32_t id, const CoordsXYZ& position)
    {
        Prune();
        auto* sound = Find(id);
        if (sound == nullptr)
            return false;

        sound->params.hasLocation = true;
        sound->params.position = position;
        GetVolumeAndPan(sound->params, sound->volume, sound->pan);
        if (!sound->paused)
        {
            sound->channel->SetVolume(sound->volume);
            sound->channel->SetPan(sound->pan);
        }
        return true;
    }

    void SetPaused(bool paused)
    {
        if (paused == _paused)
            return;
        _paused = paused;

        for (auto& sound : _sounds)
        {
            if (paused)
            {
                if (sound.channel == nullptr || sound.channel->IsDone())
                    continue;

                sound.offset = sound.channel->GetOffset();
                sound.loop = sound.channel->GetLoop();
                sound.channel->Stop();
                sound.channel = nullptr;
                sound.paused = true;
            }
            else if (sound.paused)
            {
                sound.paused = false;
                if (sound.params.hasLocation)
                    GetVolumeAndPan(sound.params, sound.volume, sound.pan);

                sound.channel = CreateAudioChannel(
                    sound.source, sound.group, sound.loop != 0, sound.volume, sound.pan, sound.rate, true);
                if (sound.channel == nullptr)
                    continue;

                if (sound.loop > 0)
                    sound.channel->SetLoop(sound.loop);
                sound.channel->SetOffset(sound.offset);
            }
        }
        Prune();
    }

    void Update()
    {
        if (_paused)
            return;

        Prune();
        for (auto& sound : _sounds)
        {
            if (!sound.params.hasLocation || sound.channel == nullptr)
                continue;

            int32_t volume = 0;
            float pan = 0;
            GetVolumeAndPan(sound.params, volume, pan);
            if (volume == sound.volume && pan == sound.pan)
                continue;

            sound.volume = volume;
            sound.pan = pan;
            sound.channel->SetVolume(volume);
            sound.channel->SetPan(pan);
        }
    }
} // namespace OpenRCT2::Audio::ManagedSounds
