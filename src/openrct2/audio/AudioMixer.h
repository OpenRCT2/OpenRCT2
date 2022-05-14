/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/IStream.hpp"

#include <memory>

#define MIXER_VOLUME_MAX 128
#define MIXER_LOOP_NONE 0
#define MIXER_LOOP_INFINITE (-1)

namespace OpenRCT2::Audio
{
    enum class SoundId : uint8_t;

    enum class MixerGroup : int32_t
    {
        Sound,
        RideMusic,
        TitleMusic,
    };

    struct IAudioSource;
    struct IAudioChannel;

    /**
     * Provides an audio stream by mixing multiple audio channels together.
     */
    struct IAudioMixer
    {
        virtual ~IAudioMixer() = default;

        virtual void Init(const char* device) abstract;
        virtual void Close() abstract;
        virtual void Lock() abstract;
        virtual void Unlock() abstract;
        virtual IAudioChannel* Play(IAudioSource* source, int32_t loop, bool deleteondone) abstract;
        virtual void Stop(IAudioChannel* channel) abstract;
        virtual void SetVolume(float volume) abstract;
    };
} // namespace OpenRCT2::Audio

#ifndef DSBPAN_LEFT
#    define DSBPAN_LEFT (-10000)
#endif
#ifndef DSBPAN_RIGHT
#    define DSBPAN_RIGHT 10000
#endif

void Mixer_Init(const char* device);
OpenRCT2::Audio::IAudioChannel* Mixer_Play_Effect(
    OpenRCT2::Audio::SoundId id, int32_t loop, int32_t volume, float pan, double rate, int32_t deleteondone);
OpenRCT2::Audio::IAudioChannel* Mixer_Play_Effect(
    OpenRCT2::Audio::IAudioSource* source, int32_t loop, int32_t volume, float pan, double rate, int32_t deleteondone);
void Mixer_Stop_Channel(void* channel);
void Mixer_Channel_Volume(void* channel, int32_t volume);
void Mixer_Channel_Pan(void* channel, float pan);
void Mixer_Channel_Rate(void* channel, double rate);
int32_t Mixer_Channel_IsPlaying(void* channel);
uint64_t Mixer_Channel_GetOffset(void* channel);
int32_t Mixer_Channel_SetOffset(void* channel, uint64_t offset);
void Mixer_Channel_SetGroup(void* channel, OpenRCT2::Audio::MixerGroup group);
OpenRCT2::Audio::IAudioChannel* Mixer_Play_Music(OpenRCT2::Audio::IAudioSource* source, int32_t loop, int32_t streaming);
void Mixer_SetVolume(float volume);

int32_t DStoMixerVolume(int32_t volume);
float DStoMixerPan(int32_t pan);
double DStoMixerRate(int32_t frequency);
