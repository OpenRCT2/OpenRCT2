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

#include <cmath>
#include <list>
#include "../core/Guard.hpp"
#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "../core/Util.hpp"
#include "AudioChannel.h"
#include "AudioSource.h"
#include "mixer.h"

extern "C"
{
    #include "../config.h"
    #include "../localisation/localisation.h"
    #include "../OpenRCT2.h"
    #include "../platform/platform.h"
    #include "../rct2.h"
    #include "audio.h"
}

IAudioMixer * gMixer;

class AudioMixer : public IAudioMixer
{
private:
    IAudioSource * _nullSource = nullptr;

    SDL_AudioDeviceID _deviceid = 0;
    AudioFormat _format = { 0 };
    std::list<IAudioChannel *> _channels;
    float _volume = 1.0f;
    float _adjust_sound_vol = 0.0f;
    float _adjust_music_vol = 0.0f;
    uint8 _setting_sound_vol = 0xFF;
    uint8 _setting_music_vol = 0xFF;

    IAudioSource * _css1sources[SOUND_MAXID] = { nullptr };
    IAudioSource * _musicsources[PATH_ID_END] = { nullptr };

    void * _channelBuffer = nullptr;
    size_t _channelBufferCapacity = 0;
    void * _convertBuffer = nullptr;
    size_t _convertBufferCapacity = 0;
    void * _effectBuffer = nullptr;
    size_t _effectBufferCapacity = 0;

public:
    AudioMixer()
    {
        _nullSource = AudioSource::CreateNull();
    }

    ~AudioMixer()
    {
        Close();
        delete _nullSource;
    }

    void Init(const char* device) override
    {
        Close();

        SDL_AudioSpec want = { 0 };
        want.freq = 44100;
        want.format = AUDIO_S16SYS;
        want.channels = 2;
        want.samples = 1024;
        want.callback = Callback;
        want.userdata = this;

        SDL_AudioSpec have;
        _deviceid = SDL_OpenAudioDevice(device, 0, &want, &have, 0);
        _format.format = have.format;
        _format.channels = have.channels;
        _format.freq = have.freq;

        LoadAllSounds();

        SDL_PauseAudioDevice(_deviceid, 0);
    }

    void Close() override
    {
        // Free channels
        Lock();
        for (IAudioChannel * channel : _channels)
        {
            delete channel;
        }
        _channels.clear();
        Unlock();

        SDL_CloseAudioDevice(_deviceid);

        // Free sources
        for (size_t i = 0; i < Util::CountOf(_css1sources); i++)
        {
            if (_css1sources[i] != _nullSource)
            {
                SafeDelete(_css1sources[i]);
            }
        }
        for (size_t i = 0; i < Util::CountOf(_musicsources); i++)
        {
            if (_musicsources[i] != _nullSource)
            {
                SafeDelete(_musicsources[i]);
            }
        }

        // Free buffers
        SafeFree(_channelBuffer);
        SafeFree(_convertBuffer);
        SafeFree(_effectBuffer);
    }

    void Lock() override
    {
        SDL_LockAudioDevice(_deviceid);
    }

    void Unlock() override
    {
        SDL_UnlockAudioDevice(_deviceid);
    }

    IAudioChannel * Play(IAudioSource * source, int loop, bool deleteondone, bool deletesourceondone) override
    {
        Lock();
        IAudioChannel * channel = AudioChannel::Create();
        if (channel != nullptr)
        {
            channel->Play(source, loop);
            channel->SetDeleteOnDone(deleteondone);
            channel->SetDeleteSourceOnDone(deletesourceondone);
            _channels.push_back(channel);
        }
        Unlock();
        return channel;
    }

    void Stop(IAudioChannel * channel) override
    {
        Lock();
        channel->SetStopping(true);
        Unlock();
    }

    bool LoadMusic(size_t pathId) override
    {
        bool result = false;
        if (pathId < Util::CountOf(_musicsources))
        {
            IAudioSource * source = _musicsources[pathId];
            if (source == nullptr)
            {
                const utf8 * path = get_file_path((int)pathId);
                source = AudioSource::CreateMemoryFromWAV(path, &_format);
                if (source == nullptr)
                {
                    source = _nullSource;
                }
                _musicsources[pathId] = source;
            }
            result = source != _nullSource;
        }
        return result;
    }

    void SetVolume(float volume) override
    {
        _volume = volume;
    }

    IAudioSource * GetSoundSource(int id) override
    {
        return _css1sources[id];
    }

    IAudioSource * GetMusicSource(int id) override
    {
        return _musicsources[id];
    }

private:
    void LoadAllSounds()
    {
        const utf8 * css1Path = get_file_path(PATH_ID_CSS1);
        for (size_t i = 0; i < Util::CountOf(_css1sources); i++)
        {
            auto source = AudioSource::CreateMemoryFromCSS1(css1Path, i, &_format);
            if (source == nullptr)
            {
                source = _nullSource;
            }
            _css1sources[i] = source;
        }
    }

    static void SDLCALL Callback(void * arg, uint8 * stream, int length)
    {
        auto mixer = static_cast<AudioMixer *>(arg);

        memset(stream, 0, length);
        std::list<IAudioChannel *>::iterator it = mixer->_channels.begin();
        while (it != mixer->_channels.end())
        {
            IAudioChannel * channel = *it;

            int group = channel->GetGroup();
            if (group != MIXER_GROUP_SOUND || gConfigSound.sound_enabled)
            {
                mixer->MixChannel(channel, stream, length);
            }
            if ((channel->IsDone() && channel->DeleteOnDone()) || channel->IsStopping())
            {
                delete channel;
                it = mixer->_channels.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    void MixChannel(IAudioChannel * channel, uint8 * data, int length)
    {
        // Did the volume level get changed? Recalculate level in this case.
        if (_setting_sound_vol != gConfigSound.sound_volume)
        {
            _setting_sound_vol = gConfigSound.sound_volume;
            _adjust_sound_vol = powf(_setting_sound_vol / 100.f, 10.f / 6.f);
        }
        if (_setting_music_vol != gConfigSound.ride_music_volume)
        {
            _setting_music_vol = gConfigSound.ride_music_volume;
            _adjust_music_vol = powf(_setting_music_vol / 100.f, 10.f / 6.f);
        }

        int sampleSize = _format.channels * _format.BytesPerSample();
        int samples = length / sampleSize;
        double rate = 1;
        if (_format.format == AUDIO_S16SYS)
        {
            rate = channel->GetRate();
        }
        int samplestoread = (int)(samples * rate);

        bool mustConvert = false;
        SDL_AudioCVT cvt;
        cvt.len_ratio = 1;
        AudioFormat streamformat = channel->GetFormat();
        if (streamformat != _format)
        {
            if (SDL_BuildAudioCVT(&cvt, streamformat.format, streamformat.channels, streamformat.freq, _format.format, _format.channels, _format.freq) == -1)
            {
                // Unable to convert channel data
                return;
            }
            mustConvert = true;
        }

        // Read raw PCM from channel
        size_t toread = (size_t)(samplestoread / cvt.len_ratio) * sampleSize;
        if (_channelBuffer == nullptr || _channelBufferCapacity < toread)
        {
            _channelBuffer = realloc(_channelBuffer, toread);
            _channelBufferCapacity = toread;
        }
        size_t bytesRead = channel->Read(_channelBuffer, toread);

        // Convert data to required format if necessary
        void * buffer = nullptr;
        size_t bufferLen = 0;
        if (mustConvert)
        {
            if (Convert(&cvt, _channelBuffer, bytesRead))
            {
                buffer = cvt.buf;
                bufferLen = cvt.len_cvt;
            }
            else
            {
                return;
            }
        }
        else
        {
            buffer = _channelBuffer;
            bufferLen = bytesRead;
        }

        // Apply effects
        size_t effectBufferLen;
        bool useBufferLengthForRatio = (bytesRead == toread);
        if (ApplyResample(channel, samples, sampleSize, buffer, bufferLen, &effectBufferLen, useBufferLengthForRatio))
        {
            buffer = _effectBuffer;
            bufferLen = effectBufferLen;
        }

        // Apply panning and volume
        ApplyPan(channel, buffer, bufferLen, sampleSize);
        int mixVolume = ApplyVolume(channel, buffer, bufferLen);

        // Finally mix on to destination buffer
        size_t dstLength = Math::Min((size_t)length, bufferLen);
        SDL_MixAudioFormat(data, (const Uint8 *)buffer, _format.format, (Uint32)dstLength, mixVolume);

        channel->UpdateOldVolume();
    }

    bool ApplyResample(IAudioChannel * channel, int samples, int sampleSize, const void * buffer, size_t bufferLen, size_t * newLength, bool useBufferLengthForRatio)
    {
        bool applied = false;
        double rate = channel->GetRate();
        if (rate != 1 && _format.format == AUDIO_S16SYS)
        {
            int inLen = (int)((double)bufferLen / sampleSize);
            int outLen = samples;

            SpeexResamplerState * resampler = channel->GetResampler();
            if (resampler == nullptr)
            {
                resampler = speex_resampler_init(_format.channels, _format.freq, _format.freq, 0, 0);
                channel->SetResampler(resampler);
            }
            if (useBufferLengthForRatio)
            {
                // use buffer lengths for conversion ratio so that it fits exactly
                speex_resampler_set_rate(resampler, inLen, samples);
            }
            else
            {
                // reached end of stream so we cant use buffer length as resampling ratio
                speex_resampler_set_rate(resampler, _format.freq, (int)(_format.freq * (1 / rate)));
            }

            size_t effectBufferReqLen  = outLen * sampleSize;
            if (_effectBuffer == nullptr || _effectBufferCapacity < effectBufferReqLen)
            {
                _effectBuffer = realloc(_effectBuffer, effectBufferReqLen);
                _effectBufferCapacity = effectBufferReqLen;
            }

            speex_resampler_process_interleaved_int(
                resampler,
                (const spx_int16_t *)buffer,
                (spx_uint32_t *)&inLen,
                (spx_int16_t *)_effectBuffer,
                (spx_uint32_t *)&outLen);
            *newLength = effectBufferReqLen;
            applied = true;
        }
        return applied;
    }

    void ApplyPan(const IAudioChannel * channel, void * buffer, size_t len, size_t sampleSize)
    {
        if (channel->GetPan() != 0.5f && _format.channels == 2)
        {
            switch (_format.format) {
            case AUDIO_S16SYS:
                EffectPanS16(channel, (sint16 *)buffer, (int)(len / sampleSize));
                break;
            case AUDIO_U8:
                EffectPanU8(channel, (uint8 *)buffer, (int)(len / sampleSize));
                break;
            }
        }
    }

    int ApplyVolume(const IAudioChannel * channel, void * buffer, size_t len)
    {
        float volumeAdjust = _volume;
        volumeAdjust *= (gConfigSound.master_volume / 100.0f);
        switch (channel->GetGroup()) {
        case MIXER_GROUP_SOUND:
            volumeAdjust *= _adjust_sound_vol;

            // Cap sound volume on title screen so music is more audible
            if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
            {
                volumeAdjust = Math::Min(volumeAdjust, 0.75f);
            }
            break;
        case MIXER_GROUP_RIDE_MUSIC:
            volumeAdjust *= _adjust_music_vol;
            break;
        }

        int startVolume = (int)(channel->GetOldVolume() * volumeAdjust);
        int endVolume = (int)(channel->GetVolume() * volumeAdjust);
        if (channel->IsStopping())
        {
            endVolume = 0;
        }

        int mixVolume = (int)(channel->GetVolume() * volumeAdjust);
        if (startVolume != endVolume)
        {
            // Set to max since we are adjusting the volume ourselves
            mixVolume = SDL_MIX_MAXVOLUME;

            // Fade between volume levels to smooth out sound and minimize clicks from sudden volume changes
            int fadeLength = (int)len / _format.BytesPerSample();
            switch (_format.format) {
            case AUDIO_S16SYS:
                EffectFadeS16((sint16 *)buffer, fadeLength, startVolume, endVolume);
                break;
            case AUDIO_U8:
                EffectFadeU8((uint8 *)buffer, fadeLength, startVolume, endVolume);
                break;
            }
        }
        return mixVolume;
    }

    static void EffectPanS16(const IAudioChannel * channel, sint16 * data, int length)
    {
        const float dt = 1.0f / (length * 2);
        float volumeL = channel->GetOldVolumeL();
        float volumeR = channel->GetOldVolumeR();
        const float d_left = dt * (channel->GetVolumeL() - channel->GetOldVolumeL());
        const float d_right = dt * (channel->GetVolumeR() - channel->GetOldVolumeR());

        for (int i = 0; i < length * 2; i += 2)
        {
            data[i] = (sint16)(data[i] * volumeL);
            data[i + 1] = (sint16)(data[i + 1] * volumeR);
            volumeL += d_left;
            volumeR += d_right;
        }
    }

    static void EffectPanU8(const IAudioChannel * channel, uint8 * data, int length)
    {
        float volumeL = channel->GetVolumeL();
        float volumeR = channel->GetVolumeR();
        float oldVolumeL = channel->GetOldVolumeL();
        float oldVolumeR = channel->GetOldVolumeR();

        for (int i = 0; i < length * 2; i += 2)
        {
            float t = (float)i / (length * 2);
            data[i] = (uint8)(data[i] * ((1.0 - t) * oldVolumeL + t * volumeL));
            data[i + 1] = (uint8)(data[i + 1] * ((1.0 - t) * oldVolumeR + t * volumeR));
        }
    }

    static void EffectFadeS16(sint16 * data, int length, int startvolume, int endvolume)
    {
        float startvolume_f = (float)startvolume / SDL_MIX_MAXVOLUME;
        float endvolume_f = (float)endvolume / SDL_MIX_MAXVOLUME;
        for (int i = 0; i < length; i++)
        {
            float t = (float)i / length;
            data[i] = (sint16)(data[i] * ((1 - t) * startvolume_f + t * endvolume_f));
        }
    }

    static void EffectFadeU8(uint8* data, int length, int startvolume, int endvolume)
    {
        float startvolume_f = (float)startvolume / SDL_MIX_MAXVOLUME;
        float endvolume_f = (float)endvolume / SDL_MIX_MAXVOLUME;
        for (int i = 0; i < length; i++)
        {
            float t = (float)i / length;
            data[i] = (uint8)(data[i] * ((1 - t) * startvolume_f + t * endvolume_f));
        }
    }

    bool Convert(SDL_AudioCVT * cvt, const void * src, size_t len)
    {
        // tofix: there seems to be an issue with converting audio using SDL_ConvertAudio in the callback vs preconverted, can cause pops and static depending on sample rate and channels
        bool result = false;
        if (len != 0 && cvt->len_mult != 0)
        {
            if (_convertBuffer == nullptr || _convertBufferCapacity < len)
            {
                _convertBufferCapacity = len * cvt->len_mult;
                _convertBuffer = realloc(_convertBuffer, _convertBufferCapacity);
            }
            memcpy(_convertBuffer, src, len);

            cvt->len = (int)len;
            cvt->buf = (Uint8 *)_convertBuffer;
            if (SDL_ConvertAudio(cvt) >= 0)
            {
                result = true;
            }
        }
        return result;
    }
};

void Mixer_Init(const char * device)
{
    if (!gOpenRCT2Headless)
    {
        gMixer = new AudioMixer();
        gMixer->Init(device);
    }
}

void * Mixer_Play_Effect(size_t id, int loop, int volume, float pan, double rate, int deleteondone)
{
    IAudioChannel * channel = nullptr;
    if (!gOpenRCT2Headless && gConfigSound.sound_enabled)
    {
        if (id >= SOUND_MAXID)
        {
            log_error("Tried to play an invalid sound id. %i", id);
        }
        else
        {
            IAudioMixer * mixer = gMixer;
            mixer->Lock();
            IAudioSource * source = mixer->GetSoundSource((int)id);
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
    if (!gOpenRCT2Headless)
    {
        gMixer->Stop(static_cast<IAudioChannel*>(channel));
    }
}

void Mixer_Channel_Volume(void * channel, int volume)
{
    if (!gOpenRCT2Headless)
    {
        gMixer->Lock();
        static_cast<IAudioChannel*>(channel)->SetVolume(volume);
        gMixer->Unlock();
    }
}

void Mixer_Channel_Pan(void * channel, float pan)
{
    if (!gOpenRCT2Headless)
    {
        gMixer->Lock();
        static_cast<IAudioChannel*>(channel)->SetPan(pan);
        gMixer->Unlock();
    }
}

void Mixer_Channel_Rate(void* channel, double rate)
{
    if (!gOpenRCT2Headless)
    {
        gMixer->Lock();
        static_cast<IAudioChannel*>(channel)->SetRate(rate);
        gMixer->Unlock();
    }
}

int Mixer_Channel_IsPlaying(void * channel)
{
    bool isPlaying = false;
    if (!gOpenRCT2Headless)
    {
        isPlaying = static_cast<IAudioChannel*>(channel)->IsPlaying();
    }
    return isPlaying;
}

unsigned long Mixer_Channel_GetOffset(void * channel)
{
    unsigned long offset = 0;
    if (!gOpenRCT2Headless)
    {
        offset = static_cast<IAudioChannel*>(channel)->GetOffset();
    }
    return offset;
}

int Mixer_Channel_SetOffset(void * channel, unsigned long offset)
{
    int result = 0;
    if (!gOpenRCT2Headless)
    {
        result = static_cast<IAudioChannel*>(channel)->SetOffset(offset);
    }
    return result;
}

void Mixer_Channel_SetGroup(void * channel, int group)
{
    if (!gOpenRCT2Headless)
    {
        static_cast<IAudioChannel *>(channel)->SetGroup(group);
    }
}

void * Mixer_Play_Music(int pathId, int loop, int streaming)
{
    IAudioChannel * channel = nullptr;
    if (!gOpenRCT2Headless)
    {
        IAudioMixer * mixer = gMixer;
        if (streaming)
        {
            const utf8 * filename = get_file_path(pathId);

            SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
            if (rw != nullptr)
            {
                auto source = AudioSource::CreateStreamFromWAV(rw);
                if (source != nullptr)
                {
                    channel = mixer->Play(source, loop, false, true);
                    if (channel == nullptr)
                    {
                        delete source;
                    }
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
    }
    return channel;
}

void Mixer_SetVolume(float volume)
{
    if (!gOpenRCT2Headless)
    {
        gMixer->SetVolume(volume);
    }
}
