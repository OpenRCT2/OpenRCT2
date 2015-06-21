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
#include <SDL.h>

#define USE_MIXER

#define MIXER_LOOP_NONE			0
#define MIXER_LOOP_INFINITE		-1

enum {
	MIXER_GROUP_NONE,
	MIXER_GROUP_MUSIC,
};

#ifdef __cplusplus

#include <list>
extern "C" {
#include <speex/speex_resampler.h>
}

struct AudioFormat {
	int BytesPerSample() const { return (SDL_AUDIO_BITSIZE(format)) / 8; };
	int freq;
	SDL_AudioFormat format;
	int channels;
};

class Source
{
public:
	virtual ~Source();
	unsigned long GetSome(unsigned long offset, const uint8** data, unsigned long length);
	unsigned long Length();
	const AudioFormat& Format();

	friend class Mixer;

protected:
	virtual unsigned long Read(unsigned long offset, const uint8** data, unsigned long length) = 0;

	AudioFormat format;
	unsigned long length;
};

class Source_Null : public Source
{
public:
	Source_Null();

protected:
	unsigned long Read(unsigned long offset, const uint8** data, unsigned long length);
};

class Source_Sample : public Source
{
public:
	Source_Sample();
	~Source_Sample();
	bool LoadWAV(const char* filename);
	bool LoadCSS1(const char* filename, unsigned int offset);

	friend class Mixer;

protected:
	bool Convert(AudioFormat format);

private:
	void Unload();

	uint8* data;
	bool issdlwav;

protected:
	unsigned long Read(unsigned long offset, const uint8** data, unsigned long length);
};

class Source_SampleStream : public Source
{
public:
	Source_SampleStream();
	~Source_SampleStream();
	bool LoadWAV(SDL_RWops* rw);

private:
	Uint32 FindChunk(SDL_RWops* rw, Uint32 wanted_id);
	void Unload();

	SDL_RWops* rw;
	Uint64 databegin;
	uint8* buffer;
	unsigned long buffersize;

protected:
	unsigned long Read(unsigned long offset, const uint8** data, unsigned long length);
};

class Channel
{
public:
	Channel();
	~Channel();
	void Play(Source& source, int loop);
	void SetRate(double rate);
	void SetVolume(int volume);
	void SetPan(float pan);
	bool IsPlaying();
	unsigned long GetOffset();
	bool SetOffset(unsigned long offset);
	void SetGroup(int group);

	friend class Mixer;

private:
	int loop;
	unsigned long offset;
	double rate;
	int volume;
	float volume_l, volume_r;
	float oldvolume_l, oldvolume_r;
	float pan;
	bool done;
	bool deleteondone;
	bool deletesourceondone;
	bool stopping;
	int oldvolume;
	int group;
	SpeexResamplerState* resampler;
	Source* source;
};

class Mixer
{
public:
	Mixer();
	void Init(const char* device);
	void Close();
	void Lock();
	void Unlock();
	Channel* Play(Source& source, int loop, bool deleteondone, bool deletesourceondone);
	void Stop(Channel& channel);
	bool LoadMusic(int pathid);

	Source* css1sources[SOUND_MAXID];
	Source* musicsources[PATH_ID_END];

private:
	static void SDLCALL Callback(void* arg, uint8* data, int length);
	void MixChannel(Channel& channel, uint8* buffer, int length);
	void EffectPanS16(Channel& channel, sint16* data, int length);
	void EffectPanU8(Channel& channel, uint8* data, int length);
	void EffectFadeS16(sint16* data, int length, int startvolume, int endvolume);
	void EffectFadeU8(uint8* data, int length, int startvolume, int endvolume);
	bool MustConvert(Source& source);
	bool Convert(SDL_AudioCVT& cvt, const uint8* data, unsigned long length, uint8** dataout);
	SDL_AudioDeviceID deviceid;
	AudioFormat format;
	uint8* effectbuffer;
	std::list<Channel*> channels;
	Source_Null source_null;
};

extern "C"
{
#endif

void Mixer_Init(const char* device);
void* Mixer_Play_Effect(int id, int loop, int volume, float pan, double rate, int deleteondone);
void Mixer_Stop_Channel(void* channel);
void Mixer_Channel_Volume(void* channel, int volume);
void Mixer_Channel_Pan(void* channel, float pan);
void Mixer_Channel_Rate(void* channel, double rate);
int Mixer_Channel_IsPlaying(void* channel);
unsigned long Mixer_Channel_GetOffset(void* channel);
int Mixer_Channel_SetOffset(void* channel, unsigned long offset);
void Mixer_Channel_SetGroup(void* channel, int group);
void* Mixer_Play_Music(int pathid, int loop, int streaming);

static int DStoMixerVolume(int volume) { return (int)(SDL_MIX_MAXVOLUME * (SDL_pow(10, (float)volume / 2000))); };
static float DStoMixerPan(int pan) { return (((float)pan + -DSBPAN_LEFT) / DSBPAN_RIGHT) / 2; };
static double DStoMixerRate(int frequency) { return (double)frequency / 22050; };

#ifdef __cplusplus
}
#endif

#endif