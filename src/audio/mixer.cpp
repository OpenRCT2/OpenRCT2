/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <math.h>
#include <SDL.h>

extern "C" {
	#include "../config.h"
	#include "../platform/platform.h"
	#include "audio.h"
	#include "../localisation/localisation.h"
}
#include "mixer.h"

Mixer gMixer;

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

	utf8 utf8filename[512];
	win1252_to_utf8(utf8filename, filename, sizeof(utf8filename));

	Unload();
	SDL_RWops* rw = SDL_RWFromFile(utf8filename, "rb");
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

bool Source_Sample::LoadCSS1(const char* filename, unsigned int offset)
{
	log_verbose("Source_Sample::LoadCSS1(%s, %d)", filename, offset);

	utf8 utf8filename[512];
	win1252_to_utf8(utf8filename, filename, sizeof(utf8filename));

	Unload();
	SDL_RWops* rw = SDL_RWFromFile(utf8filename, "rb");
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
	WAVEFORMATEX waveformat;
	SDL_RWread(rw, &waveformat, sizeof(waveformat), 1);
	format.freq = waveformat.nSamplesPerSec;
	format.format = AUDIO_S16LSB;
	format.channels = waveformat.nChannels;
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

Source_SampleStream::Source_SampleStream()
{
	length = 0;
	rw = NULL;
	buffer = 0;
	buffersize = 0;
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
		currentposition = newposition;
	}
	*data = buffer;
	int read = SDL_RWread(rw, buffer, 1, length);
	if (read == -1) {
		return 0;
	}
	return read;
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
	PCMWAVEFORMAT waveformat;
	SDL_RWread(rw, &waveformat, sizeof(waveformat), 1);
	SDL_RWseek(rw, chunkstart + fmtchunk_size, RW_SEEK_SET);
	if (waveformat.wf.wFormatTag != WAVE_FORMAT_PCM) {
		log_verbose("Not in proper format");
		return false;
	}
	format.freq = waveformat.wf.nSamplesPerSec;
	switch (waveformat.wBitsPerSample) {
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
	format.channels = waveformat.wf.nChannels;
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
	resampler = 0;
	SetRate(1);
	SetVolume(SDL_MIX_MAXVOLUME);
	oldvolume = 0;
	oldvolume_l = 0;
	oldvolume_r = 0;
	SetPan(0.5f);
	done = true;
	stopping = false;
	source = 0;
	deletesourceondone = false;
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
	double decibels = (abs(Channel::pan - 0.5) * 2.0) * 100.0;
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

Mixer::Mixer()
{
	effectbuffer = 0;
	for (int i = 0; i < countof(css1sources); i++) {
		css1sources[i] = 0;
	}
	for (int i = 0; i < countof(musicsources); i++) {
		musicsources[i] = 0;
	}
}

void Mixer::Init(const char* device)
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
	for (int i = 0; i < countof(css1sources); i++) {
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

void Mixer::Close()
{
	Lock();
	while (channels.begin() != channels.end()) {
		delete *(channels.begin());
		channels.erase(channels.begin());
	}
	Unlock();
	SDL_CloseAudioDevice(deviceid);
	for (int i = 0; i < countof(css1sources); i++) {
		if (css1sources[i] && css1sources[i] != &source_null) {
			delete css1sources[i];
			css1sources[i] = 0;
		}
	}
	for (int i = 0; i < countof(musicsources); i++) {
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

void Mixer::Lock()
{
	SDL_LockAudioDevice(deviceid);
}

void Mixer::Unlock()
{
	SDL_UnlockAudioDevice(deviceid);
}

Channel* Mixer::Play(Source& source, int loop, bool deleteondone, bool deletesourceondone)
{
	Lock();
	Channel* newchannel = new (std::nothrow) Channel;
	if (newchannel) {
		newchannel->Play(source, loop);
		newchannel->deleteondone = deleteondone;
		newchannel->deletesourceondone = deletesourceondone;
		newchannel->stopping = false;
		channels.push_back(newchannel);
	}
	Unlock();
	return newchannel;
}

void Mixer::Stop(Channel& channel)
{
	Lock();
	channel.stopping = true;
	Unlock();
}

bool Mixer::LoadMusic(int pathid)
{
	if (pathid >= countof(musicsources)) {
		return false;
	}
	if (!musicsources[pathid]) {
		const char* filename = get_file_path(pathid);
		Source_Sample* source_sample = new Source_Sample;
		if (source_sample->LoadWAV(filename)) {
			musicsources[pathid] = source_sample;
			return true;
		} else {
			delete source_sample;
			musicsources[pathid] = &source_null;
			return false;
		}
	} else {
		return true;
	}
}

void SDLCALL Mixer::Callback(void* arg, uint8* stream, int length)
{
	Mixer* mixer = (Mixer*)arg;
	memset(stream, 0, length);
	std::list<Channel*>::iterator i = mixer->channels.begin();
	while (i != mixer->channels.end()) {
		mixer->MixChannel(*(*i), stream, length);
		if (((*i)->done && (*i)->deleteondone) || (*i)->stopping) {
			delete (*i);
			i = mixer->channels.erase(i);
		} else {
			i++;
		}
	}
}

void Mixer::MixChannel(Channel& channel, uint8* data, int length)
{
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
					if (SDL_BuildAudioCVT(&cvt, streamformat.format, streamformat.channels, streamformat.freq, Mixer::format.format, Mixer::format.channels, Mixer::format.freq) == -1) {
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

				int volume = channel.volume;
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

				int startvolume = channel.oldvolume;
				int endvolume = channel.volume;
				if (channel.stopping) {
					endvolume = 0;
				}
				int mixvolume = volume;
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
		} while(loaded < length && channel.loop != 0 && !channel.stopping);

		channel.oldvolume = channel.volume;
		channel.oldvolume_l = channel.volume_l;
		channel.oldvolume_r = channel.volume_r;
		if (channel.loop == 0 && channel.offset >= channel.source->Length()) {
			channel.done = true;
		}
	}
}

void Mixer::EffectPanS16(Channel& channel, sint16* data, int length)
{
	for (int i = 0; i < length * 2; i += 2) {
		float t = (float)i / (length * 2);
		data[i] = (sint16)(data[i] * ((1.0 - t) * channel.oldvolume_l + t * channel.volume_l));
		data[i + 1] = (sint16)(data[i + 1] * ((1.0 - t) * channel.oldvolume_r + t * channel.volume_r));
	}
}

void Mixer::EffectPanU8(Channel& channel, uint8* data, int length)
{
	for (int i = 0; i < length * 2; i += 2) {
		float t = (float)i / (length * 2);
		data[i] = (uint8)(data[i] * ((1.0 - t) * channel.oldvolume_l + t * channel.volume_l));
		data[i + 1] = (uint8)(data[i + 1] * ((1.0 - t) * channel.oldvolume_r + t * channel.volume_r));
	}
}

void Mixer::EffectFadeS16(sint16* data, int length, int startvolume, int endvolume)
{
	float startvolume_f = (float)startvolume / SDL_MIX_MAXVOLUME;
	float endvolume_f = (float)endvolume / SDL_MIX_MAXVOLUME;
	for (int i = 0; i < length; i++) {
		float t = (float)i / length;
		data[i] = (sint16)(data[i] * ((1 - t) * startvolume_f + t * endvolume_f));
	}
}

void Mixer::EffectFadeU8(uint8* data, int length, int startvolume, int endvolume)
{
	float startvolume_f = (float)startvolume / SDL_MIX_MAXVOLUME;
	float endvolume_f = (float)endvolume / SDL_MIX_MAXVOLUME;
	for (int i = 0; i < length; i++) {
		float t = (float)i / length;
		data[i] = (uint8)(data[i] * ((1 - t) * startvolume_f + t * endvolume_f));
	}
}

bool Mixer::MustConvert(Source& source)
{
	const AudioFormat sourceformat = source.Format();
	if (sourceformat.format != format.format || sourceformat.channels != format.channels || sourceformat.freq != format.freq) {
		return true;
	}
	return false;
}

bool Mixer::Convert(SDL_AudioCVT& cvt, const uint8* data, unsigned long length, uint8** dataout)
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

void Mixer_Init(const char* device)
{
	gMixer.Init(device);
}

void* Mixer_Play_Effect(int id, int loop, int volume, float pan, double rate, int deleteondone)
{
	if (id >= countof(gMixer.css1sources)) {
		return 0;
	}
	gMixer.Lock();
	Channel* channel = gMixer.Play(*gMixer.css1sources[id], loop, deleteondone != 0, false);
	if (channel) {
		channel->SetVolume(volume);
		channel->SetPan(pan);
		channel->SetRate(rate);
	}
	gMixer.Unlock();
	return channel;
}

void Mixer_Stop_Channel(void* channel)
{
	gMixer.Stop(*(Channel*)channel);
}

void Mixer_Channel_Volume(void* channel, int volume)
{
	gMixer.Lock();
	((Channel*)channel)->SetVolume(volume);
	gMixer.Unlock();
}

void Mixer_Channel_Pan(void* channel, float pan)
{
	gMixer.Lock();
	((Channel*)channel)->SetPan(pan);
	gMixer.Unlock();
}

void Mixer_Channel_Rate(void* channel, double rate)
{
	gMixer.Lock();
	((Channel*)channel)->SetRate(rate);
	gMixer.Unlock();
}

int Mixer_Channel_IsPlaying(void* channel)
{
	return ((Channel*)channel)->IsPlaying();
}

unsigned long Mixer_Channel_GetOffset(void* channel)
{
	return ((Channel*)channel)->GetOffset();
}

int Mixer_Channel_SetOffset(void* channel, unsigned long offset)
{
	return ((Channel*)channel)->SetOffset(offset);
}

void* Mixer_Play_Music(int pathid, int loop, int streaming)
{
	if (streaming) {
		const char* filename = get_file_path(pathid);

		utf8 utf8filename[512];
		win1252_to_utf8(utf8filename, filename, sizeof(utf8filename));

		SDL_RWops* rw = SDL_RWFromFile(utf8filename, "rb");
		if (rw == NULL) {
			return 0;
		}
		Source_SampleStream* source_samplestream = new Source_SampleStream;
		if (source_samplestream->LoadWAV(rw)) {
			Channel* channel = gMixer.Play(*source_samplestream, loop, false, true);
			if (!channel) {
				delete source_samplestream;
			}
			return channel;
		} else {
			delete source_samplestream;
			return 0;
		}
	} else {
		if (gMixer.LoadMusic(pathid)) {
			return gMixer.Play(*gMixer.musicsources[pathid], MIXER_LOOP_INFINITE, false, false);
		}
	}
	return 0;
}