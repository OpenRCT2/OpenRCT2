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

#ifndef _MIXER_H_
#define _MIXER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "../common.h"
#include <SDL.h>
#include "../platform/platform.h"
#ifdef __cplusplus
}
#endif // __cplusplus

#define MIXER_LOOP_NONE			0
#define MIXER_LOOP_INFINITE		-1

enum {
	MIXER_GROUP_SOUND,
	MIXER_GROUP_RIDE_MUSIC,
	MIXER_GROUP_TITLE_MUSIC,
};

#ifdef __cplusplus

#include <list>
extern "C" {
#include <speex/speex_resampler.h>
}

typedef struct AudioFormat {
	int BytesPerSample() const { return (SDL_AUDIO_BITSIZE(format)) / 8; }
	int freq;
	SDL_AudioFormat format;
	int channels;
} AudioFormat;

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
	Source_SampleStream() = default;
	~Source_SampleStream();
	bool LoadWAV(SDL_RWops* rw);

private:
	Uint32 FindChunk(SDL_RWops* rw, Uint32 wanted_id);
	void Unload();

	SDL_RWops* rw = nullptr;
	Uint64 databegin = 0;
	uint8* buffer = nullptr;
	unsigned long buffersize = 0;

protected:
	unsigned long Read(unsigned long offset, const uint8** data, unsigned long length);
};

class Channel
{
public:
	Channel();
	~Channel();

};

interface IAudioChannel
{
    virtual ~IAudioChannel() = default;

    virtual Source * GetSource() const abstract;

    virtual SpeexResamplerState * GetResampler() const abstract;
    virtual void SetResampler(SpeexResamplerState * value) abstract;

    virtual int     GetGroup() const abstract;
    virtual void    SetGroup(int group) abstract;

    virtual double  GetRate() const abstract;
    virtual void    SetRate(double rate) abstract;

    virtual unsigned long   GetOffset() const abstract;
    virtual bool            SetOffset(unsigned long offset) abstract;

    virtual int     GetLoop() const abstract;
    virtual void    SetLoop(int value) abstract;

    virtual int     GetVolume() const abstract;
    virtual float   GetVolumeL() const abstract;
    virtual float   GetVolumeR() const abstract;
    virtual float   GetOldVolumeL() const abstract;
    virtual float   GetOldVolumeR() const abstract;
    virtual int     GetOldVolume() const abstract;
    virtual void    SetVolume(int volume) abstract;

    virtual float   GetPan() const abstract;
    virtual void    SetPan(float pan) abstract;

    virtual bool IsStopping() const abstract;
    virtual void SetStopping(bool value) abstract;

    virtual bool IsDone() const abstract;
    virtual void SetDone(bool value) abstract;

    virtual bool DeleteOnDone() const abstract;
    virtual void SetDeleteOnDone(bool value) abstract;

    virtual void SetDeleteSourceOnDone(bool value) abstract;

    virtual bool IsPlaying() const abstract;

    virtual void Play(Source& source, int loop = MIXER_LOOP_NONE) abstract;
    virtual void UpdateOldVolume() abstract;
};

interface IAudioMixer
{
    virtual ~IAudioMixer() = default;

    virtual void Init(const char * device) abstract;
    virtual void Close() abstract;
    virtual void Lock() abstract;
    virtual void Unlock() abstract;
    virtual IAudioChannel * Play(Source& source, int loop, bool deleteondone, bool deletesourceondone) abstract;
    virtual void Stop(IAudioChannel * channel) abstract;
    virtual bool LoadMusic(size_t pathid) abstract;
    virtual void SetVolume(float volume) abstract;

    virtual Source * GetSoundSource(int id) abstract;
    virtual Source * GetMusicSource(int id) abstract;
};

extern "C"
{
#endif

#ifndef DSBPAN_LEFT
#define DSBPAN_LEFT -10000
#endif
#ifndef DSBPAN_RIGHT
#define DSBPAN_RIGHT 10000
#endif

void Mixer_Init(const char* device);
void* Mixer_Play_Effect(size_t id, int loop, int volume, float pan, double rate, int deleteondone);
void Mixer_Stop_Channel(void* channel);
void Mixer_Channel_Volume(void* channel, int volume);
void Mixer_Channel_Pan(void* channel, float pan);
void Mixer_Channel_Rate(void* channel, double rate);
int Mixer_Channel_IsPlaying(void* channel);
unsigned long Mixer_Channel_GetOffset(void* channel);
int Mixer_Channel_SetOffset(void* channel, unsigned long offset);
void Mixer_Channel_SetGroup(void* channel, int group);
void* Mixer_Play_Music(int pathId, int loop, int streaming);
void Mixer_SetVolume(float volume);

static int DStoMixerVolume(int volume) { return (int)(SDL_MIX_MAXVOLUME * (SDL_pow(10, (float)volume / 2000))); }
static float DStoMixerPan(int pan) { return (((float)pan + -DSBPAN_LEFT) / DSBPAN_RIGHT) / 2; }
static double DStoMixerRate(int frequency) { return (double)frequency / 22050; }

#ifdef __cplusplus
}
#endif

#endif
