/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioMixer.h"

#include "../Context.h"
#include "../config/Config.h"
#include "../object/AudioObject.h"
#include "../object/ObjectManager.h"
#include "AudioChannel.h"
#include "AudioContext.h"
#include "AudioSource.h"
#include "audio.h"

#include <cmath>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

static IAudioMixer* GetMixer()
{
    auto audioContext = GetContext()->GetAudioContext();
    return audioContext->GetMixer();
}

void Mixer_Init(const char* device)
{
    auto audioContext = GetContext()->GetAudioContext();
    if (device == nullptr)
    {
        device = "";
    }
    audioContext->SetOutputDevice(std::string(device));
}

IAudioChannel* Mixer_Play_Effect(SoundId id, int32_t loop, int32_t volume, float pan, double rate, int32_t deleteondone)
{
    if (gConfigSound.sound_enabled)
    {
        // Get sound from base object
        auto baseAudioObject = GetBaseAudioObject();
        if (baseAudioObject != nullptr)
        {
            auto source = baseAudioObject->GetSample(EnumValue(id));
            if (source != nullptr)
            {
                return Mixer_Play_Effect(source, loop, volume, pan, rate, deleteondone);
            }
        }
    }
    return nullptr;
}

IAudioChannel* Mixer_Play_Effect(
    IAudioSource* source, int32_t loop, int32_t volume, float pan, double rate, int32_t deleteondone)
{
    IAudioChannel* channel = nullptr;
    if (gConfigSound.sound_enabled)
    {
        IAudioMixer* mixer = GetMixer();
        if (mixer != nullptr)
        {
            mixer->Lock();
            channel = mixer->Play(source, loop, deleteondone != 0);
            if (channel != nullptr)
            {
                channel->SetVolume(volume);
                channel->SetPan(pan);
                channel->SetRate(rate);
                channel->UpdateOldVolume();
            }
            mixer->Unlock();
        }
    }
    return channel;
}

void Mixer_Stop_Channel(void* channel)
{
    auto mixer = GetMixer();
    if (mixer != nullptr)
    {
        mixer->Stop(static_cast<IAudioChannel*>(channel));
    }
}

void Mixer_Channel_Volume(void* channel, int32_t volume)
{
    IAudioMixer* audioMixer = GetMixer();
    if (audioMixer != nullptr)
    {
        audioMixer->Lock();
        static_cast<IAudioChannel*>(channel)->SetVolume(volume);
        audioMixer->Unlock();
    }
}

void Mixer_Channel_Pan(void* channel, float pan)
{
    IAudioMixer* audioMixer = GetMixer();
    if (audioMixer != nullptr)
    {
        audioMixer->Lock();
        static_cast<IAudioChannel*>(channel)->SetPan(pan);
        audioMixer->Unlock();
    }
}

void Mixer_Channel_Rate(void* channel, double rate)
{
    IAudioMixer* audioMixer = GetMixer();
    if (audioMixer != nullptr)
    {
        audioMixer->Lock();
        static_cast<IAudioChannel*>(channel)->SetRate(rate);
        audioMixer->Unlock();
    }
}

int32_t Mixer_Channel_IsPlaying(void* channel)
{
    return static_cast<IAudioChannel*>(channel)->IsPlaying();
}

uint64_t Mixer_Channel_GetOffset(void* channel)
{
    return static_cast<IAudioChannel*>(channel)->GetOffset();
}

int32_t Mixer_Channel_SetOffset(void* channel, uint64_t offset)
{
    return static_cast<IAudioChannel*>(channel)->SetOffset(offset);
}

void Mixer_Channel_SetGroup(void* channel, MixerGroup group)
{
    static_cast<IAudioChannel*>(channel)->SetGroup(group);
}

template<typename T> static void* PlayMusic(T&& src, int32_t loop)
{
    auto* mixer = GetMixer();
    if (mixer == nullptr)
        return nullptr;

    auto audioContext = GetContext()->GetAudioContext();
    auto stream = audioContext->CreateStreamFromWAV(std::forward<T&&>(src));
    if (stream == nullptr)
        return nullptr;

    auto* channel = mixer->Play(stream, loop, false);
    if (channel == nullptr)
    {
        delete stream;
        return nullptr;
    }

    channel->SetGroup(MixerGroup::RideMusic);
    return channel;
}

IAudioChannel* Mixer_Play_Music(IAudioSource* source, int32_t loop, int32_t streaming)
{
    auto* mixer = GetMixer();
    if (mixer == nullptr)
    {
        return nullptr;
    }

    auto* channel = mixer->Play(source, loop, false);
    if (channel != nullptr)
    {
        channel->SetGroup(MixerGroup::RideMusic);
    }
    return channel;
}

void* Mixer_Play_Music(std::unique_ptr<IStream> stream, int32_t loop)
{
    return PlayMusic(std::move(stream), loop);
}

void Mixer_SetVolume(float volume)
{
    GetMixer()->SetVolume(volume);
}

int32_t DStoMixerVolume(int32_t volume)
{
    return static_cast<int32_t>(MIXER_VOLUME_MAX * (std::pow(10.0f, static_cast<float>(volume) / 2000)));
}

float DStoMixerPan(int32_t pan)
{
    return ((static_cast<float>(pan) + -DSBPAN_LEFT) / DSBPAN_RIGHT) / 2;
}

double DStoMixerRate(int32_t frequency)
{
    return static_cast<double>(frequency) / 22050;
}
