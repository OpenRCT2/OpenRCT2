/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cmath>
#include "../config/Config.h"
#include "../Context.h"
#include "audio.h"
#include "AudioChannel.h"
#include "AudioContext.h"
#include "AudioMixer.h"
#include "AudioSource.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

static IAudioMixer * GetMixer()
{
    auto audioContext = GetContext()->GetAudioContext();
    return audioContext->GetMixer();
}

void Mixer_Init(const char * device)
{
    auto audioContext = GetContext()->GetAudioContext();
    if (device == nullptr)
    {
        device = "";
    }
    audioContext->SetOutputDevice(std::string(device));
}

void * Mixer_Play_Effect(size_t id, int32_t loop, int32_t volume, float pan, double rate, int32_t deleteondone)
{
    IAudioChannel * channel = nullptr;
    if (gConfigSound.sound_enabled)
    {
        if (id >= SOUND_MAXID)
        {
            log_error("Tried to play an invalid sound id. %i", id);
        }
        else
        {
            IAudioMixer * mixer = GetMixer();
            if (mixer != nullptr)
            {
                mixer->Lock();
                IAudioSource * source = mixer->GetSoundSource((int32_t)id);
                channel = mixer->Play(source, loop, deleteondone != 0, false);
                if (channel != nullptr)
                {
                    channel->SetVolume(volume);
                    channel->SetPan(pan);
                    channel->SetRate(rate);
                }
                mixer->Unlock();
            }
        }
    }
    return channel;
}

void Mixer_Stop_Channel(void * channel)
{
    auto mixer = GetMixer();
    if (mixer != nullptr)
    {
        mixer->Stop(static_cast<IAudioChannel*>(channel));
    }
}

void Mixer_Channel_Volume(void * channel, int32_t volume)
{
    IAudioMixer * audioMixer = GetMixer();
    if (audioMixer != nullptr)
    {
        audioMixer->Lock();
        static_cast<IAudioChannel*>(channel)->SetVolume(volume);
        audioMixer->Unlock();
    }
}

void Mixer_Channel_Pan(void * channel, float pan)
{
    IAudioMixer * audioMixer = GetMixer();
    if (audioMixer != nullptr)
    {
        audioMixer->Lock();
        static_cast<IAudioChannel*>(channel)->SetPan(pan);
        audioMixer->Unlock();
    }
}

void Mixer_Channel_Rate(void* channel, double rate)
{
    IAudioMixer * audioMixer = GetMixer();
    if (audioMixer != nullptr)
    {
        audioMixer->Lock();
        static_cast<IAudioChannel*>(channel)->SetRate(rate);
        audioMixer->Unlock();
    }
}

int32_t Mixer_Channel_IsPlaying(void * channel)
{
    return static_cast<IAudioChannel*>(channel)->IsPlaying();
}

uint64_t Mixer_Channel_GetOffset(void * channel)
{
    return static_cast<IAudioChannel*>(channel)->GetOffset();
}

int32_t Mixer_Channel_SetOffset(void * channel, uint64_t offset)
{
    return static_cast<IAudioChannel*>(channel)->SetOffset(offset);
}

void Mixer_Channel_SetGroup(void * channel, int32_t group)
{
    static_cast<IAudioChannel *>(channel)->SetGroup(group);
}

void * Mixer_Play_Music(int32_t pathId, int32_t loop, int32_t streaming)
{
    IAudioChannel * channel = nullptr;
    IAudioMixer * mixer = GetMixer();
    if (mixer != nullptr)
    {
        if (streaming)
        {
            const utf8 * path = context_get_path_legacy(pathId);

            auto audioContext = GetContext()->GetAudioContext();
            IAudioSource * source = audioContext->CreateStreamFromWAV(path);
            if (source != nullptr)
            {
                channel = mixer->Play(source, loop, false, true);
                if (channel == nullptr)
                {
                    delete source;
                }
            }
        }
        else
        {
            if (mixer->LoadMusic(pathId))
            {
                IAudioSource * source = mixer->GetMusicSource(pathId);
                channel = mixer->Play(source, MIXER_LOOP_INFINITE, false, false);
            }
        }
    }
    if (channel != nullptr)
    {
        channel->SetGroup(MIXER_GROUP_RIDE_MUSIC);
    }
    return channel;
}

void Mixer_SetVolume(float volume)
{
    GetMixer()->SetVolume(volume);
}

int32_t DStoMixerVolume(int32_t volume)
{
    return (int32_t)(MIXER_VOLUME_MAX * (std::pow(10.0f, (float)volume / 2000)));
}

float DStoMixerPan(int32_t pan)
{
    return (((float)pan + -DSBPAN_LEFT) / DSBPAN_RIGHT) / 2;
}

double DStoMixerRate(int32_t frequency)
{
    return (double)frequency / 22050;
}
