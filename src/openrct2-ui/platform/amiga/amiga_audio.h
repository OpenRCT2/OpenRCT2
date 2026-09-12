/*****************************************************************************
 * AmigaOS audio output through ahi.device (AHI): a small double-buffered
 * CMD_WRITE stream that the main loop pumps. Plain C, no threads: the game has
 * no gthreads on libnix, so the mixer callback runs on the main task whenever
 * a buffer has been played and needs refilling.
 *****************************************************************************/
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/* Fill callback: write exactly `bytes` bytes of interleaved signed 16-bit big-endian stereo into `dst`. */
typedef void (*amiga_audio_fill_fn)(void* user, unsigned char* dst, int bytes);

/* 1 if ahi.device can be opened (AHI installed and a unit configured), else 0. */
int amiga_audio_available(void);

/* Open unit 0 for `freq` Hz stereo 16-bit output with `numBuffers` buffers of `frames` frames each.
 * Returns 1 on success. */
int amiga_audio_open(int freq, int frames, int numBuffers);

/* Refill and queue every buffer that has finished playing. Returns the number of buffers refilled. */
int amiga_audio_pump(amiga_audio_fill_fn fill, void* user);

/* Stop playback and free everything. Safe to call when not open. */
void amiga_audio_close(void);

/* Diagnostics: buffers written / underruns (gaps where all buffers had drained). */
void amiga_audio_stats(unsigned* written, unsigned* underruns);
void amiga_audio_debug(unsigned* reaped, int* lastError, int* inflight);
/* dos GetVar wrapper: 1 if the variable exists (value copied to buf). */
int amiga_audio_getenv(const char* name, char* buf, int len);

#ifdef __cplusplus
}
#endif
