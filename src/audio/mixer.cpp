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

#include "../core/Guard.hpp"
extern "C" {
	#include "../config.h"
	#include "../localisation/localisation.h"
	#include "../OpenRCT2.h"
	#include "../platform/platform.h"
	#include "../rct2.h"
	#include "audio.h"
}
#include "mixer.h"
#include <cmath>
#include "../core/Math.hpp"
#include "../core/Util.hpp"

IAudioMixer * gMixer;

Source::~Source()
{

}

unsigned long Source::GetSome(unsigned long offset, const uint8** data, unsigned long length)
{
	if (offset >= Length()) {
		return 0;
	}
	unsigned long size = length;
	if (offset + length > Length()) {
		size = Length() - offset;
	}
	return Read(offset, data, size);
}

unsigned long Source::Length()
{
	return length;
}

const AudioFormat& Source::Format()
{
	return format;
}

Source_Null::Source_Null()
{
	length = 0;
}

unsigned long Source_Null::Read(unsigned long offset, const uint8** data, unsigned long length)
{
	return 0;
}

Source_Sample::Source_Sample()
{
	data = 0;
	length = 0;
	issdlwav = false;
}

Source_Sample::~Source_Sample()
{
	Unload();
}

unsigned long Source_Sample::Read(unsigned long offset, const uint8** data, unsigned long length)
{
	*data = &Source_Sample::data[offset];
	return length;
}

bool Source_Sample::LoadWAV(const char* filename)
{
	log_verbose("Source_Sample::LoadWAV(%s)", filename);

	Unload();
	SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
	if (rw == NULL) {
		log_verbose("Error loading %s", filename);
		return false;
	}

	SDL_AudioSpec audiospec;
	memset(&audiospec, 0, sizeof(audiospec));
	SDL_AudioSpec* spec = SDL_LoadWAV_RW(rw, false, &audiospec, &data, (Uint32*)&length);
	SDL_RWclose(rw);

	if (spec != NULL) {
		format.freq = spec->freq;
		format.format = spec->format;
		format.channels = spec->channels;
		issdlwav = true;
	} else {
		log_verbose("Error loading %s, unsupported WAV format", filename);
		return false;
	}

	return true;
}

bool Source_Sample::LoadCSS1(const char *filename, unsigned int offset)
{
	log_verbose("Source_Sample::LoadCSS1(%s, %d)", filename, offset);

	Unload();
	SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
	if (rw == NULL) {
		log_verbose("Unable to load %s", filename);
		return false;
	}

	Uint32 numsounds;
	SDL_RWread(rw, &numsounds, sizeof(numsounds), 1);
	if (offset > numsounds) {
		SDL_RWclose(rw);
		return false;
	}
	SDL_RWseek(rw, offset * 4, RW_SEEK_CUR);
	Uint32 soundoffset;
	SDL_RWread(rw, &soundoffset, sizeof(soundoffset), 1);
	SDL_RWseek(rw, soundoffset, RW_SEEK_SET);
	Uint32 soundsize;
	SDL_RWread(rw, &soundsize, sizeof(soundsize), 1);
	length = soundsize;
#pragma pack(push, 1)
	struct WaveFormatEx
	{
		Uint16 encoding;
		Uint16 channels;
		Uint32 frequency;
		Uint32 byterate;
		Uint16 blockalign;
		Uint16 bitspersample;
		Uint16 extrasize;
	} waveformat;
#pragma pack(pop)
	assert_struct_size(waveformat, 18);
	SDL_RWread(rw, &waveformat, sizeof(waveformat), 1);
	format.freq = waveformat.frequency;
	format.format = AUDIO_S16LSB;
	format.channels = waveformat.channels;
	data = new (std::nothrow) uint8[length];
	if (!data) {
		log_verbose("Unable to allocate data");
		SDL_RWclose(rw);
		return false;
	}
	SDL_RWread(rw, data, length, 1);
	SDL_RWclose(rw);
	return true;
}

void Source_Sample::Unload()
{
	if (data) {
		if (issdlwav) {
			SDL_FreeWAV(data);
		} else {
			delete[] data;
		}
		data = 0;
	}
	issdlwav = false;
	length = 0;
}

bool Source_Sample::Convert(AudioFormat format)
{
	if(Source_Sample::format.format != format.format || Source_Sample::format.channels != format.channels || Source_Sample::format.freq != format.freq){
		SDL_AudioCVT cvt;
		if (SDL_BuildAudioCVT(&cvt, Source_Sample::format.format, Source_Sample::format.channels, Source_Sample::format.freq, format.format, format.channels, format.freq) < 0) {
			return false;
		}
		cvt.len = length;
		cvt.buf = (Uint8*)new uint8[cvt.len * cvt.len_mult];
		memcpy(cvt.buf, data, length);
		if (SDL_ConvertAudio(&cvt) < 0) {
			delete[] cvt.buf;
			return false;
		}
		Unload();
		data = cvt.buf;
		length = cvt.len_cvt;
		Source_Sample::format = format;
		return true;
	}
	return false;
}

Source_SampleStream::~Source_SampleStream()
{
	Unload();
}

unsigned long Source_SampleStream::Read(unsigned long offset, const uint8** data, unsigned long length)
{
	if (length > buffersize) {
		if (buffer) {
			delete[] buffer;
		}
		buffer = new (std::nothrow) uint8[length];
		if (!buffer) {
			return 0;
		}
		buffersize = length;
	}
	Sint64 currentposition = SDL_RWtell(rw);
	if (currentposition == -1) {
		return 0;
	}
	if (currentposition - databegin != offset) {
		Sint64 newposition = SDL_RWseek(rw, databegin + offset, SEEK_SET);
		if (newposition == -1) {
			return 0;
		}
	}
	*data = buffer;
	size_t read = SDL_RWread(rw, buffer, 1, length);
	if (read == (size_t)-1) {
		return 0;
	}
	return (unsigned long)read;
}

bool Source_SampleStream::LoadWAV(SDL_RWops* rw)
{
	Unload();
	if (rw == NULL) {
		return false;
	}
	Source_SampleStream::rw = rw;
	Uint32 chunk_id = SDL_ReadLE32(rw);
	const Uint32 RIFF = 0x46464952;
	if (chunk_id != RIFF) {
		log_verbose("Not a WAV file");
		return false;
	}
	Uint32 chunk_size = SDL_ReadLE32(rw);
	(void)chunk_size;
	Uint32 chunk_format = SDL_ReadLE32(rw);
	const Uint32 WAVE = 0x45564157;
	if (chunk_format != WAVE) {
		log_verbose("Not in WAVE format");
		return false;
	}
	const Uint32 FMT = 0x20746D66;
	Uint32 fmtchunk_size = FindChunk(rw, FMT);
	if (!fmtchunk_size) {
		log_verbose("Could not find FMT chunk");
		return false;
	}
	Uint64 chunkstart = SDL_RWtell(rw);
#pragma pack(push, 1)
	struct WaveFormat
	{
		Uint16 encoding;
		Uint16 channels;
		Uint32 frequency;
		Uint32 byterate;
		Uint16 blockalign;
		Uint16 bitspersample;
	} waveformat;
#pragma pack(pop)
	assert_struct_size(waveformat, 16);
	SDL_RWread(rw, &waveformat, sizeof(waveformat), 1);
	SDL_RWseek(rw, chunkstart + fmtchunk_size, RW_SEEK_SET);
	const Uint16 pcmformat = 0x0001;
	if (waveformat.encoding != pcmformat) {
		log_verbose("Not in proper format");
		return false;
	}
	format.freq = waveformat.frequency;
	switch (waveformat.bitspersample) {
		case 8:
			format.format = AUDIO_U8;
			break;
		case 16:
			format.format = AUDIO_S16LSB;
			break;
		default:
			log_verbose("Invalid bits per sample");
			return false;
			break;
	}
	format.channels = waveformat.channels;
	const Uint32 DATA = 0x61746164;
	Uint32 datachunk_size = FindChunk(rw, DATA);
	if (!datachunk_size) {
		log_verbose("Could not find DATA chunk");
		return false;
	}
	length = datachunk_size;
	databegin = SDL_RWtell(rw);
	return true;
}

Uint32 Source_SampleStream::FindChunk(SDL_RWops* rw, Uint32 wanted_id)
{
	Uint32 subchunk_id = SDL_ReadLE32(rw);
	Uint32 subchunk_size = SDL_ReadLE32(rw);
	if (subchunk_id == wanted_id) {
		return subchunk_size;
	}
	const Uint32 FACT = 0x74636166;
	const Uint32 LIST = 0x5453494c;
	const Uint32 BEXT = 0x74786562;
	const Uint32 JUNK = 0x4B4E554A;
	while (subchunk_id == FACT || subchunk_id == LIST || subchunk_id == BEXT || subchunk_id == JUNK) {
		SDL_RWseek(rw, subchunk_size, RW_SEEK_CUR);
		subchunk_id = SDL_ReadLE32(rw);
		subchunk_size = SDL_ReadLE32(rw);
		if (subchunk_id == wanted_id) {
			return subchunk_size;
		}
	}
	return 0;
}

void Source_SampleStream::Unload()
{
	if (rw) {
		SDL_RWclose(rw);
		rw = NULL;
	}
	length = 0;
	if (buffer) {
		delete[] buffer;
		buffer = 0;
	}
	buffersize = 0;
}

Channel::Channel()
{
	SetRate(1);
	SetVolume(SDL_MIX_MAXVOLUME);
	SetPan(0.5f);
}

Channel::~Channel()
{
	if (resampler) {
		speex_resampler_destroy(resampler);
		resampler = 0;
	}
	if (deletesourceondone) {
		delete source;
	}
}

void Channel::Play(Source& source, int loop = MIXER_LOOP_NONE)
{
	Channel::source = &source;
	Channel::loop = loop;
	offset = 0;
	done = false;
}

void Channel::SetRate(double rate)
{
	Channel::rate = rate;
	if (Channel::rate < 0.001) {
		Channel::rate = 0.001;
	}
}

void Channel::SetVolume(int volume)
{
	Channel::volume = volume;
	if (volume > SDL_MIX_MAXVOLUME) {
		Channel::volume = SDL_MIX_MAXVOLUME;
	}
	if (volume < 0) {
		Channel::volume = 0;
	}
}

void Channel::SetPan(float pan)
{
	Channel::pan = pan;
	if (pan > 1) {
		Channel::pan = 1;
	}
	if (pan < 0) {
		Channel::pan = 0;
	}
	double decibels = (std::abs(Channel::pan - 0.5) * 2.0) * 100.0;
	double attenuation = pow(10, decibels / 20.0);
	if (Channel::pan <= 0.5) {
		volume_l = 1.0;
		volume_r = float(1.0 / attenuation);
	} else {
		volume_r = 1.0;
		volume_l = float(1.0 / attenuation);
	}
}

bool Channel::IsPlaying()
{
	return !done;
}

unsigned long Channel::GetOffset()
{
	return offset;
}

bool Channel::SetOffset(unsigned long offset)
{
	if (source && offset < source->Length()) {
		int samplesize = source->Format().channels * source->Format().BytesPerSample();
		Channel::offset = (offset / samplesize) * samplesize;
		return true;
	}
	return false;
}

void Channel::SetGroup(int group)
{
	Channel::group = group;
}

class AudioMixer : public IAudioMixer
{
private:
    SDL_AudioDeviceID deviceid = 0;
    AudioFormat format = { 0 };
    uint8 * effectbuffer = nullptr;
    std::list<Channel *> channels;
    Source_Null source_null;
    float volume = 1.0f;
    float adjust_sound_vol = 0.0f;
    float adjust_music_vol = 0.0f;
    uint8 setting_sound_vol = 0xFF;
    uint8 setting_music_vol = 0xFF;

    Source * css1sources[SOUND_MAXID];
    Source * musicsources[PATH_ID_END];

public:
    AudioMixer::AudioMixer()
    {
        for (size_t i = 0; i < Util::CountOf(css1sources); i++) {
            css1sources[i] = 0;
        }
        for (size_t i = 0; i < Util::CountOf(musicsources); i++) {
            musicsources[i] = 0;
        }
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
        deviceid = SDL_OpenAudioDevice(device, 0, &want, &have, 0);
        format.format = have.format;
        format.channels = have.channels;
        format.freq = have.freq;
        const char* filename = get_file_path(PATH_ID_CSS1);
        for (int i = 0; i < (int)Util::CountOf(css1sources); i++) {
            Source_Sample* source_sample = new Source_Sample;
            if (source_sample->LoadCSS1(filename, i)) {
                source_sample->Convert(format); // convert to audio output format, saves some cpu usage but requires a bit more memory, optional
                css1sources[i] = source_sample;
            } else {
                css1sources[i] = &source_null;
                delete source_sample;
            }
        }
        effectbuffer = new uint8[(have.samples * format.BytesPerSample() * format.channels)];
        SDL_PauseAudioDevice(deviceid, 0);
    }

    void Close() override
    {
        Lock();
        while (channels.begin() != channels.end()) {
            delete *(channels.begin());
            channels.erase(channels.begin());
        }
        Unlock();
        SDL_CloseAudioDevice(deviceid);
        for (size_t i = 0; i < Util::CountOf(css1sources); i++) {
            if (css1sources[i] && css1sources[i] != &source_null) {
                delete css1sources[i];
                css1sources[i] = 0;
            }
        }
        for (size_t i = 0; i < Util::CountOf(musicsources); i++) {
            if (musicsources[i] && musicsources[i] != &source_null) {
                delete musicsources[i];
                musicsources[i] = 0;
            }
        }
        if (effectbuffer) {
            delete[] effectbuffer;
            effectbuffer = 0;
        }
    }

    void Lock() override
    {
        SDL_LockAudioDevice(deviceid);
    }

    void Unlock() override
    {
        SDL_UnlockAudioDevice(deviceid);
    }

    Channel * Play(Source& source, int loop, bool deleteondone, bool deletesourceondone) override
    {
        Lock();
        Channel* newchannel = new (std::nothrow) Channel;
        if (newchannel) {
            newchannel->Play(source, loop);
            newchannel->deleteondone = deleteondone;
            newchannel->deletesourceondone = deletesourceondone;
            channels.push_back(newchannel);
        }
        Unlock();
        return newchannel;
    }

    void Stop(Channel& channel) override
    {
        Lock();
        channel.stopping = true;
        Unlock();
    }

    bool LoadMusic(size_t pathId) override
    {
        if (pathId >= Util::CountOf(musicsources)) {
            return false;
        }
        if (!musicsources[pathId]) {
            const char* filename = get_file_path((int)pathId);
            Source_Sample* source_sample = new Source_Sample;
            if (source_sample->LoadWAV(filename)) {
                musicsources[pathId] = source_sample;
                return true;
            } else {
                delete source_sample;
                musicsources[pathId] = &source_null;
                return false;
            }
        } else {
            return true;
        }
    }

    void SetVolume(float volume) override
    {
        this->volume = volume;
    }

    Source * GetSoundSource(int id) override
    {
        return css1sources[id];
    }

    Source * GetMusicSource(int id) override
    {
        return musicsources[id];
    }

private:
    static void SDLCALL Callback(void * arg, uint8 * stream, int length)
    {
        auto mixer = static_cast<AudioMixer *>(arg);
        memset(stream, 0, length);
        std::list<Channel*>::iterator i = mixer->channels.begin();
        while (i != mixer->channels.end()) {
            Channel * channel = *i;
            mixer->MixChannel(*channel, stream, length);
            if ((channel->done && channel->deleteondone) || channel->stopping)
            {
                delete channel;
                i = mixer->channels.erase(i);
            }
            else
            {
                i++;
            }
        }
    }

    void MixChannel(Channel& channel, uint8* data, int length) 
    {
        // Did the volume level get changed? Recalculate level in this case.
        if (setting_sound_vol != gConfigSound.sound_volume) {
            setting_sound_vol = gConfigSound.sound_volume;
            adjust_sound_vol = powf(setting_sound_vol / 100.f, 10.f / 6.f);
        }
        if (setting_music_vol != gConfigSound.ride_music_volume) {
            setting_music_vol = gConfigSound.ride_music_volume;
            adjust_music_vol = powf(setting_music_vol / 100.f, 10.f / 6.f);
        }

        // Do not mix channel if channel is a sound and sound is disabled
        if (channel.group == MIXER_GROUP_SOUND && !gConfigSound.sound_enabled) {
            return;
        }

        if (channel.source && channel.source->Length() > 0 && !channel.done) {
            AudioFormat streamformat = channel.source->Format();
            int loaded = 0;
            SDL_AudioCVT cvt;
            cvt.len_ratio = 1;
            do {
                int samplesize = format.channels * format.BytesPerSample();
                int samples = length / samplesize;
                int samplesloaded = loaded / samplesize;
                double rate = 1;
                if (format.format == AUDIO_S16SYS) {
                    rate = channel.rate;
                }
                int samplestoread = (int)((samples - samplesloaded) * rate);
                int lengthloaded = 0;
                if (channel.offset < channel.source->Length()) {
                    bool mustconvert = false;
                    if (MustConvert(*channel.source)) {
                        if (SDL_BuildAudioCVT(&cvt, streamformat.format, streamformat.channels, streamformat.freq, format.format, format.channels, format.freq) == -1) {
                            break;
                        }
                        mustconvert = true;
                    }

                    const uint8* datastream = 0;
                    int toread = (int)(samplestoread / cvt.len_ratio) * samplesize;
                    int readfromstream = (channel.source->GetSome(channel.offset, &datastream, toread));
                    if (readfromstream == 0) {
                        break;
                    }

                    uint8* dataconverted = 0;
                    const uint8* tomix = 0;

                    if (mustconvert) {
                        // tofix: there seems to be an issue with converting audio using SDL_ConvertAudio in the callback vs preconverted, can cause pops and static depending on sample rate and channels
                        if (Convert(cvt, datastream, readfromstream, &dataconverted)) {
                            tomix = dataconverted;
                            lengthloaded = cvt.len_cvt;
                        } else {
                            break;
                        }
                    } else {
                        tomix = datastream;
                        lengthloaded = readfromstream;
                    }

                    bool effectbufferloaded = false;
                    if (rate != 1 && format.format == AUDIO_S16SYS) {
                        int in_len = (int)((double)lengthloaded / samplesize);
                        int out_len = samples;
                        if (!channel.resampler) {
                            channel.resampler = speex_resampler_init(format.channels, format.freq, format.freq, 0, 0);
                        }
                        if (readfromstream == toread) {
                            // use buffer lengths for conversion ratio so that it fits exactly
                            speex_resampler_set_rate(channel.resampler, in_len, samples - samplesloaded);
                        } else {
                            // reached end of stream so we cant use buffer length as resampling ratio
                            speex_resampler_set_rate(channel.resampler, format.freq, (int)(format.freq * (1 / rate)));
                        }
                        speex_resampler_process_interleaved_int(channel.resampler, (const spx_int16_t*)tomix, (spx_uint32_t*)&in_len, (spx_int16_t*)effectbuffer, (spx_uint32_t*)&out_len);
                        effectbufferloaded = true;
                        tomix = effectbuffer;
                        lengthloaded = (out_len * samplesize);
                    }

                    if (channel.pan != 0.5f && format.channels == 2) {
                        if (!effectbufferloaded) {
                            memcpy(effectbuffer, tomix, lengthloaded);
                            effectbufferloaded = true;
                            tomix = effectbuffer;
                        }
                        switch (format.format) {
                        case AUDIO_S16SYS:
                            EffectPanS16(channel, (sint16*)effectbuffer, lengthloaded / samplesize);
                            break;
                        case AUDIO_U8:
                            EffectPanU8(channel, (uint8*)effectbuffer, lengthloaded / samplesize);
                            break;
                        }
                    }

                    int mixlength = lengthloaded;
                    if (loaded + mixlength > length) {
                        mixlength = length - loaded;
                    }

                    float volumeadjust = volume;
                    volumeadjust *= (gConfigSound.master_volume / 100.0f);
                    switch (channel.group) {
                    case MIXER_GROUP_SOUND:
                        volumeadjust *= adjust_sound_vol;

                        // Cap sound volume on title screen so music is more audible
                        if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) {
                            volumeadjust = Math::Min(volumeadjust, 0.75f);
                        }
                        break;
                    case MIXER_GROUP_RIDE_MUSIC:
                        volumeadjust *= adjust_music_vol;
                        break;
                    }
                    int startvolume = (int)(channel.oldvolume * volumeadjust);
                    int endvolume = (int)(channel.volume * volumeadjust);
                    if (channel.stopping) {
                        endvolume = 0;
                    }
                    int mixvolume = (int)(channel.volume * volumeadjust);
                    if (startvolume != endvolume) {
                        // fade between volume levels to smooth out sound and minimize clicks from sudden volume changes
                        if (!effectbufferloaded) {
                            memcpy(effectbuffer, tomix, lengthloaded);
                            effectbufferloaded = true;
                            tomix = effectbuffer;
                        }
                        mixvolume = SDL_MIX_MAXVOLUME; // set to max since we are adjusting the volume ourselves
                        int fadelength = mixlength / format.BytesPerSample();
                        switch (format.format) {
                        case AUDIO_S16SYS:
                            EffectFadeS16((sint16*)effectbuffer, fadelength, startvolume, endvolume);
                            break;
                        case AUDIO_U8:
                            EffectFadeU8((uint8*)effectbuffer, fadelength, startvolume, endvolume);
                            break;
                        }
                    }

                    SDL_MixAudioFormat(&data[loaded], tomix, format.format, mixlength, mixvolume);

                    if (dataconverted) {
                        delete[] dataconverted;
                    }

                    channel.offset += readfromstream;
                }

                loaded += lengthloaded;

                if (channel.loop != 0 && channel.offset >= channel.source->Length()) {
                    if (channel.loop != -1) {
                        channel.loop--;
                    }
                    channel.offset = 0;
                }
            } while (loaded < length && channel.loop != 0 && !channel.stopping);

            channel.oldvolume = channel.volume;
            channel.oldvolume_l = channel.volume_l;
            channel.oldvolume_r = channel.volume_r;
            if (channel.loop == 0 && channel.offset >= channel.source->Length()) {
                channel.done = true;
            }
        }
    }

    void EffectPanS16(Channel& channel, sint16* data, int length)
    {
        const float dt = 1.0f / (length * 2);
        float left_volume = channel.oldvolume_l;
        float right_volume = channel.oldvolume_r;
        const float d_left = dt * (channel.volume_l - channel.oldvolume_l);
        const float d_right = dt * (channel.volume_r - channel.oldvolume_r);

        for (int i = 0; i < length * 2; i += 2) {
            data[i] = (sint16)(data[i] * left_volume);
            data[i + 1] = (sint16)(data[i + 1] * right_volume);
            left_volume += d_left;
            right_volume += d_right;
        }
    }

    void EffectPanU8(Channel& channel, uint8* data, int length)
    {
        for (int i = 0; i < length * 2; i += 2) {
            float t = (float)i / (length * 2);
            data[i] = (uint8)(data[i] * ((1.0 - t) * channel.oldvolume_l + t * channel.volume_l));
            data[i + 1] = (uint8)(data[i + 1] * ((1.0 - t) * channel.oldvolume_r + t * channel.volume_r));
        }
    }

    void EffectFadeS16(sint16* data, int length, int startvolume, int endvolume)
    {
        float startvolume_f = (float)startvolume / SDL_MIX_MAXVOLUME;
        float endvolume_f = (float)endvolume / SDL_MIX_MAXVOLUME;
        for (int i = 0; i < length; i++) {
            float t = (float)i / length;
            data[i] = (sint16)(data[i] * ((1 - t) * startvolume_f + t * endvolume_f));
        }
    }

    void EffectFadeU8(uint8* data, int length, int startvolume, int endvolume)
    {
        float startvolume_f = (float)startvolume / SDL_MIX_MAXVOLUME;
        float endvolume_f = (float)endvolume / SDL_MIX_MAXVOLUME;
        for (int i = 0; i < length; i++) {
            float t = (float)i / length;
            data[i] = (uint8)(data[i] * ((1 - t) * startvolume_f + t * endvolume_f));
        }
    }

    bool MustConvert(Source& source)
    {
        const AudioFormat sourceformat = source.Format();
        if (sourceformat.format != format.format || sourceformat.channels != format.channels || sourceformat.freq != format.freq) {
            return true;
        }
        return false;
    }

    bool Convert(SDL_AudioCVT& cvt, const uint8* data, unsigned long length, uint8** dataout)
    {
        if (length == 0 || cvt.len_mult == 0) {
            return false;
        }
        cvt.len = length;
        cvt.buf = (Uint8*)new uint8[cvt.len * cvt.len_mult];
        memcpy(cvt.buf, data, length);
        if (SDL_ConvertAudio(&cvt) < 0) {
            delete[] cvt.buf;
            return false;
        }
        *dataout = cvt.buf;
        return true;
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
    Channel * channel = nullptr;
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
            Source * source = mixer->GetSoundSource((int)id);
            channel = mixer->Play(*source, loop, deleteondone != 0, false);
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
        gMixer->Stop(*static_cast<Channel*>(channel));
    }
}

void Mixer_Channel_Volume(void * channel, int volume)
{
    if (!gOpenRCT2Headless)
    {
        gMixer->Lock();
        static_cast<Channel*>(channel)->SetVolume(volume);
        gMixer->Unlock();
    }
}

void Mixer_Channel_Pan(void* channel, float pan)
{
    if (!gOpenRCT2Headless)
    {
        gMixer->Lock();
        static_cast<Channel*>(channel)->SetPan(pan);
        gMixer->Unlock();
    }
}

void Mixer_Channel_Rate(void* channel, double rate)
{
    if (!gOpenRCT2Headless)
    {
        gMixer->Lock();
        static_cast<Channel*>(channel)->SetRate(rate);
        gMixer->Unlock();
    }
}

int Mixer_Channel_IsPlaying(void * channel)
{
    bool isPlaying = false;
    if (!gOpenRCT2Headless)
    {
        isPlaying = static_cast<Channel*>(channel)->IsPlaying();
    }
    return isPlaying;
}

unsigned long Mixer_Channel_GetOffset(void * channel)
{
    unsigned long offset = 0;
    if (!gOpenRCT2Headless)
    {
        offset = static_cast<Channel*>(channel)->GetOffset();
    }
    return offset;
}

int Mixer_Channel_SetOffset(void * channel, unsigned long offset)
{
    int result = 0;
    if (!gOpenRCT2Headless)
    {
        result = static_cast<Channel*>(channel)->SetOffset(offset);
    }
    return result;
}

void Mixer_Channel_SetGroup(void * channel, int group)
{
    if (!gOpenRCT2Headless)
    {
        static_cast<Channel*>(channel)->SetGroup(group);
    }
}

void * Mixer_Play_Music(int pathId, int loop, int streaming)
{
    Channel * channel = nullptr;
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
                    channel = mixer->Play(*source_samplestream, loop, false, true);
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
                Source * source = mixer->GetMusicSource(pathId);
                channel = mixer->Play(*source, MIXER_LOOP_INFINITE, false, false);
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
