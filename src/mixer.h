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

#ifndef _MIXER_H_
#define _MIXER_H_

#include "rct2.h"

#ifdef __cplusplus

extern "C" {
#include <speex/speex_resampler.h>
}

struct AudioFormat {
	int BytesPerSample() const { return (SDL_AUDIO_BITSIZE(format)) / 8; };
	int freq;
	SDL_AudioFormat format;
	Uint8 channels;
};

class Sample
{
public:
	Sample();
	~Sample();
	bool Load(char* filename);
	bool LoadCSS1(char* filename, unsigned int offset);
	void Unload();
	bool Convert(AudioFormat format);
	const Uint8* Data();
	int Length();

	friend class Stream;

private:
	AudioFormat format;
	Uint8* data;
	Uint32 length;
	bool issdlwav;
};

class Stream
{
public:
	Stream();
	int GetSome(int offset, const Uint8** data, int length);
	int Length();
	void SetSource_Sample(Sample& sample);
	const AudioFormat* Format();

	friend class Mixer;

private:
	enum {
		SOURCE_NONE = 0,
		SOURCE_SAMPLE
	} sourcetype;
	Sample* sample;
};

class Channel
{
public:
	Channel();
	~Channel();
	void Play(Stream& stream, int loop);
	void SetRate(double rate);
	void SetVolume(int volume);

	friend class Mixer;

private:
	int loop;
	int offset;
	double rate;
	int volume;
	SpeexResamplerState* resampler;
	Stream* stream;
};

class Mixer
{
public:
	void Init(char* device);
	void Close();

private:
	static void SDLCALL Callback(void* arg, Uint8* data, int length);
	void MixChannel(Channel& channel, Uint8* buffer, int length);
	bool MustConvert(Stream& stream);
	bool Convert(SDL_AudioCVT& cvt, const Uint8* data, int length, Uint8** dataout);
	SDL_AudioDeviceID deviceid;
	AudioFormat format;
	Sample css1samples[63];
	Stream css1streams[63];
	Channel channels[10];
};

extern "C"
{
#endif

void Mixer_Init(char* device);

#ifdef __cplusplus
}
#endif

#endif