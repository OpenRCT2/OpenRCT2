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

#pragma pack(push, 1)
    struct WaveFormat
    {
        Uint16 encoding;
        Uint16 channels;
        Uint32 frequency;
        Uint32 byterate;
        Uint16 blockalign;
        Uint16 bitspersample;
    };
    assert_struct_size(WaveFormat, 16);

    struct WaveFormatEx
    {
        uint16 encoding;
        uint16 channels;
        uint32 frequency;
        uint32 byterate;
        uint16 blockalign;
        uint16 bitspersample;
        uint16 extrasize;
    };
    assert_struct_size(WaveFormatEx, 18);
#pragma pack(pop)

IAudioMixer * gMixer;

// unsigned long Source::GetSome(unsigned long offset, const uint8** data, unsigned long length)
// {
// 	if (offset >= Length()) {
// 		return 0;
// 	}
// 	unsigned long size = length;
// 	if (offset + length > Length()) {
// 		size = Length() - offset;
// 	}
// 	return Read(offset, data, size);
// }

/**
 * An audio source representing silence.
 */
class Source_Null : public IAudioSource
{
public:
    size_t GetLength() override
    {
        return 0;
    }

    AudioFormat GetFormat() override
    {
        return { 0 };
    }

    size_t Read(void * dst, size_t offset, size_t len) override
    {
        return 0;
    }
};

/**
 * An audio source where raw PCM data is initially loaded into RAM from
 * a file and then streamed.
 */
class Source_Sample : public IAudioSource
{
private:
    AudioFormat _format = { 0 };
    uint8 *     _data = nullptr;
    size_t      _length = 0;
    bool        _isSDLWav = false;

public:
    ~Source_Sample()
    {
        Unload();
    }

    size_t GetLength() override
    {
        return _length;
    }

    AudioFormat GetFormat() override
    {
        return _format;
    }

    size_t Read(void * dst, size_t offset, size_t len) override
    {
        size_t bytesToRead = 0;
        if (offset < _length)
        {
            bytesToRead = Math::Min(len, _length - offset);
            Memory::Copy<void>(dst, _data + offset, bytesToRead);
        }
        return bytesToRead;
    }

    bool LoadWAV(const utf8 * path)
    {
        log_verbose("Source_Sample::LoadWAV(%s)", path);

        Unload();

        bool result = false;
        SDL_RWops * rw = SDL_RWFromFile(path, "rb");
        if (rw != nullptr)
        {
            SDL_AudioSpec audiospec = { 0 };
            Uint32 audioLen;
            SDL_AudioSpec * spec = SDL_LoadWAV_RW(rw, false, &audiospec, &_data, &audioLen);
            if (spec != nullptr)
            {
                _format.freq = spec->freq;
                _format.format = spec->format;
                _format.channels = spec->channels;
                _length = audioLen;
                _isSDLWav = true;
                result = true;
            }
            else
            {
                log_verbose("Error loading %s, unsupported WAV format", path);
            }
            SDL_RWclose(rw);
        }
        else
        {
            log_verbose("Error loading %s", path);
        }
        return result;
    }

    bool LoadCSS1(const utf8 * path, size_t index)
    {
        log_verbose("Source_Sample::LoadCSS1(%s, %d)", path, index);

        Unload();

        bool result = false;
        SDL_RWops * rw = SDL_RWFromFile(path, "rb");
        if (rw != nullptr)
        {
            uint32 numSounds;
            SDL_RWread(rw, &numSounds, sizeof(numSounds), 1);
            if (index < numSounds)
            {
                SDL_RWseek(rw, index * 4, RW_SEEK_CUR);

                uint32 pcmOffset;
                SDL_RWread(rw, &pcmOffset, sizeof(pcmOffset), 1);
                SDL_RWseek(rw, pcmOffset, RW_SEEK_SET);

                uint32 pcmSize;
                SDL_RWread(rw, &pcmSize, sizeof(pcmSize), 1);
                _length = pcmSize;

                WaveFormatEx waveFormat;
                SDL_RWread(rw, &waveFormat, sizeof(waveFormat), 1);
                _format.freq = waveFormat.frequency;
                _format.format = AUDIO_S16LSB;
                _format.channels = waveFormat.channels;

                _data = new (std::nothrow) uint8[_length];
                if (_data != nullptr)
                {
                    SDL_RWread(rw, _data, _length, 1);
                    result = true;
                }
                else
                {
                    log_verbose("Unable to allocate data");
                }
            }
            SDL_RWclose(rw);
        }
        else
        {
            log_verbose("Unable to load %s", path);
        }
        return result;
    }

    bool Convert(const AudioFormat * format)
    {
        if (_format.format != format->format ||
            _format.channels != format->channels ||
            _format.freq != format->freq)
        {
            SDL_AudioCVT cvt;
            if (SDL_BuildAudioCVT(&cvt, _format.format, _format.channels, _format.freq, format->format, format->channels, format->freq) >= 0)
            {
                cvt.len = (int)_length;
                cvt.buf = (Uint8*)new uint8[cvt.len * cvt.len_mult];
                memcpy(cvt.buf, _data, _length);
                if (SDL_ConvertAudio(&cvt) >= 0)
                {
                    Unload();
                    _data = cvt.buf;
                    _length = cvt.len_cvt;
                    _format = *format;
                    return true;
                }
                else
                {
                    delete[] cvt.buf;
                }
            }
        }
        return false;
    }

private:
    void Unload()
    {
        if (_data != nullptr)
        {
            if (_isSDLWav)
            {
                SDL_FreeWAV(_data);
            }
            else
            {
                delete[] _data;
            }
            _data = nullptr;
        }
        _isSDLWav = false;
        _length = 0;
    }
};

/**
 * An audio source where raw PCM data is streamed directly from
 * a file.
 */
class Source_SampleStream : public IAudioSource
{
private:
    AudioFormat _format = { 0 };
    SDL_RWops * _rw = nullptr;
    uint64      _dataBegin = 0;
    uint64      _dataLength = 0;

public:
    ~Source_SampleStream()
    {
        Unload();
    }

    size_t GetLength() override
    {
        return _dataLength;
    }

    AudioFormat GetFormat() override
    {
        return _format;
    }

    size_t Read(void * dst, size_t offset, size_t len) override
    {
        size_t bytesRead = 0;
        sint64 currentPosition = SDL_RWtell(_rw);
        if (currentPosition != -1)
        {
            size_t bytesToRead = Math::Min(len, _dataLength - offset);
            if (currentPosition != _dataBegin + offset)
            {
                sint64 newPosition = SDL_RWseek(_rw, _dataBegin + offset, SEEK_SET);
                if (newPosition == -1)
                {
                    return 0;
                }
            }
            bytesRead = SDL_RWread(_rw, dst, 1, bytesToRead);
        }
        return bytesRead;
    }

    bool LoadWAV(SDL_RWops * rw)
    {
        const uint32 DATA = 0x61746164;
        const Uint32 FMT  = 0x20746D66;
        const Uint32 RIFF = 0x46464952;
        const Uint32 WAVE = 0x45564157;
        const Uint16 pcmformat = 0x0001;

        Unload();

        if (rw == nullptr)
        {
            return false;
        }
        _rw = rw;

        Uint32 chunk_id = SDL_ReadLE32(rw);
        if (chunk_id != RIFF)
        {
            log_verbose("Not a WAV file");
            return false;
        }

        Uint32 chunkSize = SDL_ReadLE32(rw);
        Uint32 chunkFormat = SDL_ReadLE32(rw);
        if (chunkFormat != WAVE)
        {
            log_verbose("Not in WAVE format");
            return false;
        }

        Uint32 fmtChunkSize = FindChunk(rw, FMT);
        if (!fmtChunkSize)
        {
            log_verbose("Could not find FMT chunk");
            return false;
        }

        uint64 chunkStart = SDL_RWtell(rw);

        WaveFormat waveFormat;
        SDL_RWread(rw, &waveFormat, sizeof(waveFormat), 1);
        SDL_RWseek(rw, chunkStart + fmtChunkSize, RW_SEEK_SET);
        if (waveFormat.encoding != pcmformat) {
            log_verbose("Not in proper format");
            return false;
        }

        _format.freq = waveFormat.frequency;
        switch (waveFormat.bitspersample) {
        case 8:
            _format.format = AUDIO_U8;
            break;
        case 16:
            _format.format = AUDIO_S16LSB;
            break;
        default:
            log_verbose("Invalid bits per sample");
            return false;
            break;
        }
        _format.channels = waveFormat.channels;

        uint32 dataChunkSize = FindChunk(rw, DATA);
        if (dataChunkSize == 0)
        {
            log_verbose("Could not find DATA chunk");
            return false;
        }

        _dataLength = dataChunkSize;
        _dataBegin = SDL_RWtell(rw);
        return true;
    }

private:
    uint32 FindChunk(SDL_RWops * rw, uint32 wantedId)
    {
        uint32 subchunkId = SDL_ReadLE32(rw);
        uint32 subchunkSize = SDL_ReadLE32(rw);
        if (subchunkId == wantedId)
        {
            return subchunkSize;
        }
        const Uint32 FACT = 0x74636166;
        const Uint32 LIST = 0x5453494c;
        const Uint32 BEXT = 0x74786562;
        const Uint32 JUNK = 0x4B4E554A;
        while (subchunkId == FACT || subchunkId == LIST || subchunkId == BEXT || subchunkId == JUNK)
        {
            SDL_RWseek(rw, subchunkSize, RW_SEEK_CUR);
            subchunkId = SDL_ReadLE32(rw);
            subchunkSize = SDL_ReadLE32(rw);
            if (subchunkId == wantedId)
            {
                return subchunkSize;
            }
        }
        return 0;
    }

    void Unload()
    {
        if (_rw != nullptr)
        {
            SDL_RWclose(_rw);
            _rw = nullptr;
        }
        _dataBegin = 0;
        _dataLength = 0;
    }
};

class AudioChannel : public IAudioChannel
{
private:
    IAudioSource * _source = nullptr;
    SpeexResamplerState * _resampler = nullptr;

    int _group = MIXER_GROUP_SOUND;
    double _rate = 0;
    size_t _offset = 0;
    int _loop = 0;

    int     _volume = 1;
    float   _volume_l = 0.f;
    float   _volume_r = 0.f;
    float   _oldvolume_l = 0.f;
    float   _oldvolume_r = 0.f;
    int     _oldvolume = 0;
    float   _pan = 0;

    bool    _stopping = false;
    bool    _done = true;
    bool    _deleteondone = false;
    bool    _deletesourceondone = false;

public:
    AudioChannel()
    {
        SetRate(1);
        SetVolume(SDL_MIX_MAXVOLUME);
        SetPan(0.5f);
    }

    ~AudioChannel() override
    {
        if (_resampler != nullptr)
        {
            speex_resampler_destroy(_resampler);
            _resampler = nullptr;
        }
        if (_deletesourceondone)
        {
            delete _source;
        }
    }

    IAudioSource * GetSource() const override
    {
        return _source;
    }

    SpeexResamplerState * GetResampler() const override
    {
        return _resampler;
    }

    void SetResampler(SpeexResamplerState * value) override
    {
        _resampler = value;
    }

    int GetGroup() const override
    {
        return _group;
    }

    void SetGroup(int group)
    {
        _group = group;
    }

    double GetRate() const override
    {
        return _rate;
    }

    void SetRate(double rate)
    {
        _rate = Math::Max(0.001, rate);
    }

    unsigned long GetOffset() const override
    {
        return (unsigned long)_offset;
    }

    bool SetOffset(unsigned long offset)
    {
        if (_source != nullptr && offset < _source->GetLength())
        {
            AudioFormat format = _source->GetFormat();
            int samplesize = format.channels * format.BytesPerSample();
            _offset = (offset / samplesize) * samplesize;
            return true;
        }
        return false;
    }

    virtual int GetLoop() const override
    {
        return _loop;
    }

    virtual void SetLoop(int value) override
    {
        _loop = value;
    }

    int GetVolume() const override
    {
        return _volume;
    }

    float GetVolumeL() const override
    {
        return _volume_l;
    }

    float GetVolumeR() const override
    {
        return _volume_r;
    }

    float GetOldVolumeL() const override
    {
        return _oldvolume_l;
    }

    float GetOldVolumeR() const override
    {
        return _oldvolume_r;
    }

    int GetOldVolume() const override
    {
        return _oldvolume;
    }

    void SetVolume(int volume) override
    {
        _volume = Math::Clamp(0, volume, SDL_MIX_MAXVOLUME);
    }

    float GetPan() const override
    {
        return _pan;
    }

    void SetPan(float pan)
    {
        _pan = Math::Clamp(0.0f, pan, 1.0f);
        double decibels = (std::abs(_pan - 0.5) * 2.0) * 100.0;
        double attenuation = pow(10, decibels / 20.0);
        if (_pan <= 0.5)
        {
            _volume_l = 1.0;
            _volume_r = (float)(1.0 / attenuation);
        }
        else
        {
            _volume_r = 1.0;
            _volume_l = (float)(1.0 / attenuation);
        }
    }

    bool IsStopping() const override
    {
        return _stopping;
    }

    void SetStopping(bool value) override
    {
        _stopping = value;
    }

    bool IsDone() const override
    {
        return _done;
    }

    void SetDone(bool value) override
    {
        _done = value;
    }

    bool DeleteOnDone() const
    {
        return _deleteondone;
    }

    void SetDeleteOnDone(bool value) override
    {
        _deleteondone = value;
    }

    void SetDeleteSourceOnDone(bool value) override
    {
        _deletesourceondone = value;
    }

    bool IsPlaying() const override
    {
        return !_done;
    }

    void Play(IAudioSource * source, int loop)
    {
        _source = source;
        _loop = loop;
        _offset = 0;
        _done = false;
    }

    void UpdateOldVolume() override
    {
        _oldvolume = _volume;
        _oldvolume_l = _volume_l;
        _oldvolume_r = _volume_r;
    }

    AudioFormat GetFormat() const override
    {
        AudioFormat result = { 0 };
        if (_source != nullptr)
        {
            result = _source->GetFormat();
        }
        return result;
    }

    size_t Read(void * dst, size_t len) override
    {
        size_t bytesRead = 0;
        size_t bytesToRead = len;
        while (bytesToRead > 0 && !_done)
        {
            size_t readLen = _source->Read(dst, _offset, bytesToRead);
            if (readLen > 0)
            {
                dst = (void *)((uintptr_t)dst + readLen);
                bytesToRead -= readLen;
                bytesRead += readLen;
                _offset += readLen;
            }
            if (_offset >= _source->GetLength())
            {
                if (_loop == 0)
                {
                    _done = true;
                }
                else if (_loop == MIXER_LOOP_INFINITE)
                {
                    _offset = 0;
                }
                else
                {
                    _loop--;
                    _offset = 0;
                }
            }
        }
        return bytesRead;
    }
};

class AudioMixer : public IAudioMixer
{
private:
    SDL_AudioDeviceID _deviceid = 0;
    AudioFormat _format = { 0 };
    std::list<IAudioChannel *> _channels;
    Source_Null _source_null;
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
    }

    ~AudioMixer()
    {
        Close();
    }

    void Init(const char* device) override
    {
        Close();
        SDL_AudioSpec want, have;
        SDL_zero(want);
        want.freq = 44100;
        want.format = AUDIO_S16SYS;
        want.channels = 2;
        want.samples = 1024;
        want.callback = Callback;
        want.userdata = this;
        _deviceid = SDL_OpenAudioDevice(device, 0, &want, &have, 0);
        _format.format = have.format;
        _format.channels = have.channels;
        _format.freq = have.freq;
        const char* filename = get_file_path(PATH_ID_CSS1);
        for (int i = 0; i < (int)Util::CountOf(_css1sources); i++) {
            Source_Sample* source_sample = new Source_Sample;
            if (source_sample->LoadCSS1(filename, i)) {
                source_sample->Convert(&_format); // convert to audio output format, saves some cpu usage but requires a bit more memory, optional
                _css1sources[i] = source_sample;
            } else {
                _css1sources[i] = &_source_null;
                delete source_sample;
            }
        }
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
            if (_css1sources[i] && _css1sources[i] != &_source_null)
            {
                SafeDelete(_css1sources[i]);
            }
        }
        for (size_t i = 0; i < Util::CountOf(_musicsources); i++)
        {
            if (_musicsources[i] && _musicsources[i] != &_source_null)
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
        IAudioChannel * newchannel = new (std::nothrow) AudioChannel;
        if (newchannel != nullptr)
        {
            newchannel->Play(source, loop);
            newchannel->SetDeleteOnDone(deleteondone);
            newchannel->SetDeleteSourceOnDone(deletesourceondone);
            _channels.push_back(newchannel);
        }
        Unlock();
        return newchannel;
    }

    void Stop(IAudioChannel * channel) override
    {
        Lock();
        channel->SetStopping(true);
        Unlock();
    }

    bool LoadMusic(size_t pathId) override
    {
        if (pathId >= Util::CountOf(_musicsources))
        {
            return false;
        }
        if (!_musicsources[pathId])
        {
            const char* filename = get_file_path((int)pathId);
            Source_Sample* source_sample = new Source_Sample;
            if (source_sample->LoadWAV(filename))
            {
                _musicsources[pathId] = source_sample;
                return true;
            }
            else
            {
                delete source_sample;
                _musicsources[pathId] = &_source_null;
                return false;
            }
        }
        else
        {
            return true;
        }
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

        AudioFormat streamformat = channel->GetFormat();
        SDL_AudioCVT cvt;
        cvt.len_ratio = 1;
        int sampleSize = _format.channels * _format.BytesPerSample();
        int samples = length / sampleSize;
        double rate = 1;
        if (_format.format == AUDIO_S16SYS)
        {
            rate = channel->GetRate();
        }
        int samplestoread = (int)(samples * rate);
        bool mustConvert = false;
        if (MustConvert(&streamformat))
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

    bool MustConvert(const AudioFormat * sourceFormat)
    {
        if (sourceFormat->format != _format.format ||
            sourceFormat->channels != _format.channels ||
            sourceFormat->freq != _format.freq)
        {
            return true;
        }
        return false;
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
                Source_SampleStream * source_samplestream = new Source_SampleStream;
                if (source_samplestream->LoadWAV(rw))
                {
                    channel = mixer->Play(source_samplestream, loop, false, true);
                    if (channel == nullptr)
                    {
                        delete source_samplestream;
                    }
                }
                else
                {
                    delete source_samplestream;
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
