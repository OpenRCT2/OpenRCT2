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
#include <string.h>

extern "C" {
#include "audio.h"
#include "config.h"
}
#include "mixer.h"

Sample::Sample()
{
	data = 0;
	length = 0;
	issdlwav = false;
}

Sample::~Sample()
{
	Unload();
}

bool Sample::Load(char* filename)
{
	Unload();
	SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
	if (!rw) {
		SDL_RWclose(rw);
		return false;
	}
	SDL_AudioSpec audiospec;
	memset(&audiospec, 0, sizeof(audiospec));
	SDL_AudioSpec* spec = SDL_LoadWAV_RW(rw, false, &audiospec, &data, &length);
	if (spec != NULL) {
		format.freq = spec->freq;
		format.format = spec->format;
		format.channels = spec->channels;
	} else {
		issdlwav = true;
	}
	return true;
}

bool Sample::LoadCSS1(char* filename, unsigned int offset)
{
	Unload();
	SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
	if (!rw) {
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
	format.format = AUDIO_S16;
	format.channels = (Uint8)waveformat.nChannels;
	data = new Uint8[length];
	SDL_RWread(rw, data, length, 1);
	SDL_RWclose(rw);
	return true;
}

void Sample::Unload()
{
	SDL_LockAudio();
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
	SDL_UnlockAudio();
}

bool Sample::Convert(AudioFormat format)
{
	if(Sample::format.format != format.format || Sample::format.channels != format.channels || Sample::format.freq != format.freq){
		SDL_AudioCVT cvt;
		if (SDL_BuildAudioCVT(&cvt, Sample::format.format, Sample::format.channels, Sample::format.freq, format.format, format.channels, format.freq) < 0) {
			return false;
		}
		cvt.len = length;
		cvt.buf = new Uint8[cvt.len * cvt.len_mult];
		memcpy(cvt.buf, data, length);
		if (SDL_ConvertAudio(&cvt) < 0) {
			delete[] cvt.buf;
			return false;
		}
		Unload();
		data = cvt.buf;
		length = cvt.len_cvt;
		Sample::format = format;
	}
	return true;
}

const Uint8* Sample::Data()
{
	return data;
}

int Sample::Length()
{
	return length;
}

Stream::Stream()
{
	sourcetype = SOURCE_NONE;
}

int Stream::GetSome(int offset, const Uint8** data, int length)
{
	int size = length;
	switch(sourcetype) {
		case SOURCE_SAMPLE:
			if (offset >= sample->Length()) {
				return 0;
			}
			if (offset + length > sample->Length()) {
				size = sample->Length() - offset;
			}
			*data = &sample->Data()[offset];
			return size;
		break;
	}
	return 0;
}

int Stream::Length()
{
	switch(sourcetype) {
		case SOURCE_SAMPLE:
			return sample->Length();
		break;
	}
	return 0;
}

void Stream::SetSource_Sample(Sample& sample)
{
	sourcetype = SOURCE_SAMPLE;
	Stream::sample = &sample;
}

const AudioFormat* Stream::Format()
{
	switch(sourcetype) {
		case SOURCE_SAMPLE:
			return &sample->format;
		break;
	}
	return 0;
}

Channel::Channel()
{
	rate = 1;
	resampler = 0;
	SetVolume(SDL_MIX_MAXVOLUME);
}

Channel::~Channel()
{
	if (resampler) {
		speex_resampler_destroy(resampler);
		resampler = 0;
	}
}

void Channel::Play(Stream& stream, int loop = 0)
{
	Channel::stream = &stream;
	Channel::loop = loop;
	offset = 0;
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
	if (Channel::volume > SDL_MIX_MAXVOLUME) {
		Channel::volume = SDL_MIX_MAXVOLUME;
	}
	if (Channel::volume < 0) {
		Channel::volume = 0;
	}
}

void Mixer::Init(char* device)
{
	Close();
	SDL_AudioSpec want, have;
	SDL_zero(want);
	want.freq = 22050;
	want.format = AUDIO_S16;
	want.channels = 2;
	want.samples = 1024;
	want.callback = Callback;
	want.userdata = this;
	deviceid = SDL_OpenAudioDevice(device, 0, &want, &have, 0);
	format.format = have.format;
	format.channels = have.channels;
	format.freq = have.freq;
	char css1filename[260];
	strcpy(css1filename, gGeneral_config.game_path);
	strcat(css1filename, "\\Data\\css1.dat");
	for (int i = 0; i < 63; i++) {
		css1samples[i].LoadCSS1(css1filename, i);
		css1samples[i].Convert(format); // convert to audio output format, saves some cpu usage but requires a bit more memory, optional
		css1streams[i].SetSource_Sample(css1samples[i]);
	}
	SDL_PauseAudioDevice(deviceid, 0);
}

void Mixer::Close()
{
	SDL_CloseAudioDevice(deviceid);
}

void SDLCALL Mixer::Callback(void* arg, Uint8* stream, int length)
{
	Mixer* mixer = (Mixer*)arg;
	memset(stream, 0, length);
	for (int i = 0; i < 10; i++) {
		mixer->MixChannel(mixer->channels[i], stream, length);
	}
}

void Mixer::MixChannel(Channel& channel, Uint8* data, int length)
{
	if (channel.stream) {
		if (!channel.resampler) {
			channel.resampler = speex_resampler_init(format.channels, format.freq, format.freq, 0, 0);
		}
		AudioFormat channelformat = *channel.stream->Format();
		int loaded = 0;
		SDL_AudioCVT cvt;
		cvt.len_ratio = 1;
		do {
			int samplesize = format.channels * format.BytesPerSample();
			int samples = length / samplesize;
			int samplesloaded = loaded / samplesize;
			int samplestoread = (int)ceil((samples - samplesloaded) * channel.rate);
			int lengthloaded = 0;
			if (channel.offset < channel.stream->Length()) {
				bool mustconvert = false;
				if (MustConvert(*channel.stream)) {
					if (SDL_BuildAudioCVT(&cvt, channelformat.format, channelformat.channels, channelformat.freq, Mixer::format.format, Mixer::format.channels, Mixer::format.freq) == -1) {
						break;
					}
					mustconvert = true;
				}
				const Uint8* datastream;
				int readfromstream = (channel.stream->GetSome(channel.offset, &datastream, (int)(((samplestoread) * samplesize) / cvt.len_ratio)) / channelformat.BytesPerSample()) * channelformat.BytesPerSample();
				if (readfromstream == 0) {
					break;
				}
				int volume = channel.volume;
				if (mustconvert) {
					Uint8* dataconverted;
					if (Convert(cvt, datastream, readfromstream, &dataconverted)) {
						if (channel.rate != 1 && format.format == AUDIO_S16) {
							spx_uint32_t in_len = (spx_uint32_t)(ceil((double)cvt.len_cvt / samplesize));
							Uint8* out = new Uint8[length + 200]; // needs some extra, otherwise resampler sometimes doesn't process all the input samples
							spx_uint32_t out_len = samples + 20;
							speex_resampler_set_rate(channel.resampler, format.freq, (int)(format.freq * (1 / channel.rate)));
							speex_resampler_process_interleaved_int(channel.resampler, (const spx_int16_t*)dataconverted, &in_len, (spx_int16_t*)out, &out_len);
							int mixlength = (out_len * samplesize);
							if (loaded + mixlength > length) { // check for overflow
								mixlength = length - loaded;
							}
							lengthloaded = (out_len * samplesize);
							SDL_MixAudioFormat(&data[loaded], out, format.format, mixlength, volume);
							delete[] out;
						} else {
							lengthloaded = (cvt.len_cvt / samplesize) * samplesize;
							int mixlength = lengthloaded;
							if (loaded + cvt.len_cvt > length) {
								mixlength = length - loaded;
							}
							SDL_MixAudioFormat(&data[loaded], dataconverted, format.format, mixlength, volume);
						}
						delete[] dataconverted;
					}
				} else {
					if (channel.rate != 1 && format.format == AUDIO_S16) {
						spx_uint32_t in_len = (spx_uint32_t)(ceil((double)readfromstream / samplesize));
						Uint8* out = new Uint8[length + 200];
						spx_uint32_t out_len = samples + 20;
						speex_resampler_set_rate(channel.resampler, format.freq, (int)(format.freq * (1 / channel.rate)));
						speex_resampler_process_interleaved_int(channel.resampler, (const spx_int16_t*)datastream, &in_len, (spx_int16_t*)out, &out_len);
						int mixlength = (out_len * samplesize);
						if (loaded + mixlength > length) {
							mixlength = length - loaded;
						}
						lengthloaded = (out_len * samplesize);
						SDL_MixAudioFormat(&data[loaded], out, format.format, mixlength, volume);
						delete[] out;
					} else {
						lengthloaded = readfromstream;
						int mixlength = lengthloaded;
						if (loaded + readfromstream > length) {
							mixlength = length - loaded;
						}
						SDL_MixAudioFormat(&data[loaded], datastream, format.format, mixlength, volume);
					}
				}

				channel.offset += readfromstream;

			}

			loaded += lengthloaded;

			if (channel.loop != 0 && channel.offset >= channel.stream->Length()) {
				if (channel.loop != -1) {
					channel.loop--;
				}
				channel.offset = 0;
			}
		} while(loaded < length || (loaded < length && channel.loop != 0 && channel.offset == 0));
	}
}

bool Mixer::MustConvert(Stream& stream)
{
	const AudioFormat* streamformat = stream.Format();
	if (!streamformat) {
		return false;
	}
	if (streamformat->format != format.format || streamformat->channels != format.channels || streamformat->freq != format.freq) {
		return true;
	}
	return false;
}

bool Mixer::Convert(SDL_AudioCVT& cvt, const Uint8* data, int length, Uint8** dataout)
{
	if (length == 0 || cvt.len_mult == 0) {
		return false;
	}
	cvt.len = length;
	cvt.buf = (Uint8*)new char[cvt.len * cvt.len_mult];
	memcpy(cvt.buf, data, length);
	if (SDL_ConvertAudio(&cvt) < 0) {
		delete[] cvt.buf;
		return false;
	}
	*dataout = cvt.buf;
	return true;
}

void Mixer_Init(char* device)
{
	static Mixer mixer;
	mixer.Init(device);
}