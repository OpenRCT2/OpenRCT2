#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include "../config/Config.h"
#include "../Context.h"
#include "audio.h"
#include "AudioChannel.h"
#include "AudioContext.h"
#include "AudioMixer.h"
#include "AudioSource.h"

extern "C"
{
    #include "../rct2.h"
}

#define SDL_MIX_MAXVOLUME 128

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

static IAudioMixer * GetMixer()
{
    IAudioContext * audioContext = GetContext()->GetAudioContext();
    return audioContext->GetMixer();
}

void Mixer_Init(const char * device)
{
    IAudioContext * audioContext = GetContext()->GetAudioContext();
    if (device == nullptr)
    {
        device = "";
    }
    audioContext->SetOutputDevice(std::string(device));
}

void * Mixer_Play_Effect(size_t id, sint32 loop, sint32 volume, float pan, double rate, sint32 deleteondone)
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
            mixer->Lock();
            IAudioSource * source = mixer->GetSoundSource((sint32)id);
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
    return channel;
}

void Mixer_Stop_Channel(void * channel)
{
    GetMixer()->Stop(static_cast<IAudioChannel*>(channel));
}

void Mixer_Channel_Volume(void * channel, sint32 volume)
{
    IAudioMixer * audioMixer = GetMixer();
    audioMixer->Lock();
    static_cast<IAudioChannel*>(channel)->SetVolume(volume);
    audioMixer->Unlock();
}

void Mixer_Channel_Pan(void * channel, float pan)
{
    IAudioMixer * audioMixer = GetMixer();
    audioMixer->Lock();
    static_cast<IAudioChannel*>(channel)->SetPan(pan);
    audioMixer->Unlock();
}

void Mixer_Channel_Rate(void* channel, double rate)
{
    IAudioMixer * audioMixer = GetMixer();
    audioMixer->Lock();
    static_cast<IAudioChannel*>(channel)->SetRate(rate);
    audioMixer->Unlock();
}

sint32 Mixer_Channel_IsPlaying(void * channel)
{
    return static_cast<IAudioChannel*>(channel)->IsPlaying();
}

uint64 Mixer_Channel_GetOffset(void * channel)
{
    return static_cast<IAudioChannel*>(channel)->GetOffset();
}

sint32 Mixer_Channel_SetOffset(void * channel, uint64 offset)
{
    return static_cast<IAudioChannel*>(channel)->SetOffset(offset);
}

void Mixer_Channel_SetGroup(void * channel, sint32 group)
{
    static_cast<IAudioChannel *>(channel)->SetGroup(group);
}

void * Mixer_Play_Music(sint32 pathId, sint32 loop, sint32 streaming)
{
    IAudioChannel * channel = nullptr;
    IAudioMixer * mixer = GetMixer();
    if (streaming)
    {
        const utf8 * path = get_file_path(pathId);

        IAudioContext * audioContext = GetContext()->GetAudioContext();
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

sint32 DStoMixerVolume(sint32 volume)
{
    return (sint32)(SDL_MIX_MAXVOLUME * (std::pow(10.0f, (float)volume / 2000)));
}

float DStoMixerPan(sint32 pan)
{
    return (((float)pan + -DSBPAN_LEFT) / DSBPAN_RIGHT) / 2;
}

double DStoMixerRate(sint32 frequency)
{
    return (double)frequency / 22050;
}
