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

#include "../common.h"

#define MIXER_LOOP_NONE			0
#define MIXER_LOOP_INFINITE		-1

#ifdef __cplusplus

extern "C" {
#include <speex/speex_resampler.h>
}

struct AudioFormat {
	int BytesPerSample() const { return (SDL_AUDIO_BITSIZE(format)) / 8; };
	int freq;
	SDL_AudioFormat format;
	int channels;
};

class Sample
{
public:
	Sample();
	~Sample();
	bool Load(const char* filename);
	bool LoadCSS1(const char* filename, unsigned int offset);
	void Unload();
	bool Convert(AudioFormat format);
	const uint8* Data();
	unsigned long Length();

	friend class Stream;

private:
	AudioFormat format;
	uint8* data;
	unsigned long length;
	bool issdlwav;
};

class Stream
{
public:
	Stream();
	unsigned long GetSome(unsigned long offset, const uint8** data, unsigned long length);
	unsigned long Length();
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
	void SetPan(float pan);

	friend class Mixer;

private:
	int loop;
	unsigned long offset;
	double rate;
	int volume;
	float volume_l, volume_r;
	float pan;
	SpeexResamplerState* resampler;
	Stream* stream;
};

class Mixer
{
public:
	void Init(const char* device);
	void Close();

private:
	static void SDLCALL Callback(void* arg, uint8* data, int length);
	void MixChannel(Channel& channel, uint8* buffer, int length);
	void EffectPanS16(Channel& channel, sint16* data, int length);
	void EffectPanU8(Channel& channel, uint8* data, int length);
	bool MustConvert(Stream& stream);
	bool Convert(SDL_AudioCVT& cvt, const uint8* data, unsigned long length, uint8** dataout);
	SDL_AudioDeviceID deviceid;
	AudioFormat format;
	uint8* effectbuffer;
	Sample css1samples[SOUND_MAXID];
	Stream css1streams[SOUND_MAXID];
	Channel channels[10];
};

extern "C"
{
#endif

void Mixer_Init(const char* device);

#ifdef __cplusplus
}
#endif

#endif