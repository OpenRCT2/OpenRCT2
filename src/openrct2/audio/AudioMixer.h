#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#pragma once

#include "../common.h"

#define MIXER_VOLUME_MAX        128
#define MIXER_LOOP_NONE         0
#define MIXER_LOOP_INFINITE     -1

enum MIXER_GROUP
{
    MIXER_GROUP_SOUND,
    MIXER_GROUP_RIDE_MUSIC,
    MIXER_GROUP_TITLE_MUSIC,
};

#ifdef __cplusplus

namespace OpenRCT2 { namespace Audio
{
    interface IAudioSource;
    interface IAudioChannel;

    /**
     * Provides an audio stream by mixing multiple audio channels together.
     */
    interface IAudioMixer
    {
        virtual ~IAudioMixer() = default;

        virtual void Init(const char * device) abstract;
        virtual void Close() abstract;
        virtual void Lock() abstract;
        virtual void Unlock() abstract;
        virtual IAudioChannel * Play(IAudioSource * source, sint32 loop, bool deleteondone, bool deletesourceondone) abstract;
        virtual void Stop(IAudioChannel * channel) abstract;
        virtual bool LoadMusic(size_t pathid) abstract;
        virtual void SetVolume(float volume) abstract;

        virtual IAudioSource * GetSoundSource(sint32 id) abstract;
        virtual IAudioSource * GetMusicSource(sint32 id) abstract;
    };
} }

#endif

#ifdef __cplusplus
extern "C"
{
#endif
    #ifndef DSBPAN_LEFT
    #define DSBPAN_LEFT -10000
    #endif
    #ifndef DSBPAN_RIGHT
    #define DSBPAN_RIGHT 10000
    #endif

    void Mixer_Init(const char * device);
    void* Mixer_Play_Effect(size_t id, sint32 loop, sint32 volume, float pan, double rate, sint32 deleteondone);
    void Mixer_Stop_Channel(void* channel);
    void Mixer_Channel_Volume(void* channel, sint32 volume);
    void Mixer_Channel_Pan(void* channel, float pan);
    void Mixer_Channel_Rate(void* channel, double rate);
    sint32 Mixer_Channel_IsPlaying(void* channel);
    uint64 Mixer_Channel_GetOffset(void* channel);
    sint32 Mixer_Channel_SetOffset(void* channel, uint64 offset);
    void Mixer_Channel_SetGroup(void* channel, sint32 group);
    void* Mixer_Play_Music(sint32 pathId, sint32 loop, sint32 streaming);
    void Mixer_SetVolume(float volume);

    sint32 DStoMixerVolume(sint32 volume);
    float DStoMixerPan(sint32 pan);
    double DStoMixerRate(sint32 frequency);
#ifdef __cplusplus
}
#endif
