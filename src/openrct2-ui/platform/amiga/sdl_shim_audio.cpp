/*****************************************************************************
 * SDL2 audio + RWops subset for the AmigaOS port, on top of ahi.device
 * (amiga_audio.c). There are no threads on libnix, so the SDL audio callback
 * (OpenRCT2's mixer) runs on the main task: SDL_PollEvent calls
 * SDL_amiga_AudioPump(), which refills every AHI buffer that has finished
 * playing. Formats: the mixer works in AUDIO_S16SYS (= S16MSB on 68k); the WAV
 * and CSS sources are S16LSB/U8 and get converted by SDL_ConvertAudio.
 *****************************************************************************/
#ifdef __amigaos__
    #include "amiga_audio.h"
    #include "sdl2/SDL.h"

    #include <algorithm>
    #include <cstdio>
    #include <cstdlib>
    #include <cstring>
    #include <vector>

extern "C" void amiga_trace(const char*);

namespace
{
    SDL_AudioSpec g_spec{};
    bool g_open = false;
    bool g_paused = true;
    int g_lock = 0;
    bool g_inPump = false;
    const char* g_deviceName = "AHI (ahi.device unit 0)";
    // Debug: OPENRCT2_AUDIO_DUMP=<file> bypasses AHI. Buffers are consumed at real-time pace and the mixed
    // S16 big-endian stereo stream is appended to <file>, so the mixer can be verified on an emulator
    // whose Paula/AHI playback does not complete requests.
    bool g_dump = false;
    FILE* g_dumpFile = nullptr;
    unsigned g_dumpNextMs = 0;
    unsigned g_dumpBuffers = 0;
    unsigned g_dumpReported = 0;
    std::vector<unsigned char> g_dumpBuf;

    void fillFromCallback(void* user, unsigned char* dst, int bytes)
    {
        (void)user;
        if (g_spec.callback != nullptr)
            g_spec.callback(g_spec.userdata, dst, bytes);
        else
            std::memset(dst, 0, static_cast<size_t>(bytes));
    }

    // ---- sample format helpers -------------------------------------------------------------
    int bytesPerSample(SDL_AudioFormat f)
    {
        return SDL_AUDIO_BITSIZE(f) / 8;
    }

    // Decode one sample of `f` at `p` to a native int16.
    inline int16_t decode(SDL_AudioFormat f, const Uint8* p)
    {
        switch (f)
        {
            case AUDIO_U8:
                return static_cast<int16_t>((static_cast<int>(p[0]) - 128) << 8);
            case AUDIO_S8:
                return static_cast<int16_t>(static_cast<int8_t>(p[0]) << 8);
            case AUDIO_S16LSB:
                return static_cast<int16_t>(p[0] | (p[1] << 8));
            case AUDIO_U16LSB:
                return static_cast<int16_t>((p[0] | (p[1] << 8)) - 32768);
            case AUDIO_S16MSB:
                return static_cast<int16_t>((p[0] << 8) | p[1]);
            case AUDIO_U16MSB:
                return static_cast<int16_t>(((p[0] << 8) | p[1]) - 32768);
            default:
                return 0;
        }
    }
    inline void encode(SDL_AudioFormat f, Uint8* p, int16_t v)
    {
        switch (f)
        {
            case AUDIO_U8:
                p[0] = static_cast<Uint8>((v >> 8) + 128);
                break;
            case AUDIO_S8:
                p[0] = static_cast<Uint8>(v >> 8);
                break;
            case AUDIO_S16LSB:
                p[0] = static_cast<Uint8>(v & 0xFF);
                p[1] = static_cast<Uint8>((v >> 8) & 0xFF);
                break;
            case AUDIO_U16LSB:
            {
                uint16_t u = static_cast<uint16_t>(v + 32768);
                p[0] = static_cast<Uint8>(u & 0xFF);
                p[1] = static_cast<Uint8>(u >> 8);
                break;
            }
            case AUDIO_S16MSB:
                p[0] = static_cast<Uint8>((v >> 8) & 0xFF);
                p[1] = static_cast<Uint8>(v & 0xFF);
                break;
            case AUDIO_U16MSB:
            {
                uint16_t u = static_cast<uint16_t>(v + 32768);
                p[0] = static_cast<Uint8>(u >> 8);
                p[1] = static_cast<Uint8>(u & 0xFF);
                break;
            }
            default:
                break;
        }
    }
    bool supported(SDL_AudioFormat f)
    {
        switch (f)
        {
            case AUDIO_U8:
            case AUDIO_S8:
            case AUDIO_S16LSB:
            case AUDIO_U16LSB:
            case AUDIO_S16MSB:
            case AUDIO_U16MSB:
                return true;
            default:
                return false;
        }
    }
} // namespace

extern "C" {

// ---- device ----------------------------------------------------------------------------------
SDL_AudioDeviceID SDL_OpenAudioDevice(const char*, int iscapture, const SDL_AudioSpec* desired, SDL_AudioSpec* obtained, int)
{
    if (iscapture || desired == nullptr)
        return 0;
    SDL_CloseAudioDevice(1);
    int freq = desired->freq > 0 ? desired->freq : 22050;
    int frames = desired->samples > 0 ? desired->samples : 2048;
    frames = std::clamp(frames, 512, 4096);
    {
        char path[256];
        g_dump = amiga_audio_getenv("OPENRCT2_AUDIO_DUMP", path, sizeof path) != 0;
        if (g_dump)
        {
            g_dumpFile = std::fopen(path, "wb");
            g_dumpBuf.assign(static_cast<size_t>(frames) * 4, 0);
            g_dumpNextMs = SDL_GetTicks();
            amiga_trace("audio: DUMP mode (no AHI), writing the mixed stream to a file");
        }
    }
    if (!g_dump && !amiga_audio_open(freq, frames, 3))
    {
        amiga_trace("audio: amiga_audio_open failed (no AHI unit 0?)");
        return 0;
    }
    g_spec = *desired;
    g_spec.freq = freq;
    g_spec.format = AUDIO_S16SYS;
    g_spec.channels = 2;
    g_spec.silence = 0;
    g_spec.samples = static_cast<Uint16>(frames);
    g_spec.size = static_cast<Uint32>(frames * 4);
    if (obtained != nullptr)
        *obtained = g_spec;
    g_open = true;
    g_paused = true;
    amiga_trace("audio: AHI opened, 22050 Hz S16 stereo, 3 buffers");
    return 1;
}

void SDL_PauseAudioDevice(SDL_AudioDeviceID dev, int pause_on)
{
    if (dev == 1)
        g_paused = pause_on != 0;
}
void SDL_LockAudioDevice(SDL_AudioDeviceID dev)
{
    if (dev == 1)
        g_lock++;
}
void SDL_UnlockAudioDevice(SDL_AudioDeviceID dev)
{
    if (dev == 1 && g_lock > 0)
        g_lock--;
}
void SDL_CloseAudioDevice(SDL_AudioDeviceID dev)
{
    if (dev == 1 && g_open)
    {
        if (g_dump)
        {
            if (g_dumpFile != nullptr)
                std::fclose(g_dumpFile);
            g_dumpFile = nullptr;
        }
        else
            amiga_audio_close();
        g_open = false;
        g_paused = true;
    }
}
int SDL_GetNumAudioDevices(int iscapture)
{
    return iscapture ? 0 : 1;
}
const char* SDL_GetAudioDeviceName(int index, int iscapture)
{
    return (index == 0 && !iscapture) ? g_deviceName : nullptr;
}

void SDL_amiga_AudioPump(void)
{
    if (!g_open || g_paused || g_lock > 0 || g_inPump)
        return;
    g_inPump = true;
    const unsigned nowMs = SDL_GetTicks();
    if (g_dump)
    {
        // one buffer per (frames / freq) seconds of wall time, at most 4 per call to catch up after a stall
        const unsigned bufMs = static_cast<unsigned>(g_spec.samples) * 1000u / static_cast<unsigned>(g_spec.freq);
        int n = 0;
        while (nowMs >= g_dumpNextMs && n < 4)
        {
            fillFromCallback(nullptr, g_dumpBuf.data(), static_cast<int>(g_dumpBuf.size()));
            if (g_dumpFile != nullptr)
                std::fwrite(g_dumpBuf.data(), 1, g_dumpBuf.size(), g_dumpFile);
            g_dumpBuffers++;
            g_dumpNextMs += bufMs;
            n++;
        }
        if (nowMs > g_dumpNextMs + 2000)
            g_dumpNextMs = nowMs; // stalled for long (park load): drop the backlog
        if (g_dumpFile != nullptr && g_dumpBuffers - g_dumpReported >= 50)
        {
            g_dumpReported = g_dumpBuffers;
            std::fflush(g_dumpFile);
        }
        g_inPump = false;
        return;
    }
    amiga_audio_pump(fillFromCallback, nullptr);
    g_inPump = false;
    // Trace-gated health line every ~1000 buffers (~90 s): buffers written and underruns (all buffers drained).
    static unsigned lastReport = 0;
    unsigned written, underruns;
    amiga_audio_stats(&written, &underruns);
    if (written - lastReport >= 1000)
    {
        lastReport = written;
        char line[96];
        std::snprintf(line, sizeof line, "audio: %u buffers written, %u underruns", written, underruns);
        amiga_trace(line);
    }
}

// ---- conversion ------------------------------------------------------------------------------
int SDL_BuildAudioCVT(
    SDL_AudioCVT* cvt, SDL_AudioFormat src_format, Uint8 src_channels, int src_rate, SDL_AudioFormat dst_format,
    Uint8 dst_channels, int dst_rate)
{
    if (cvt == nullptr || !supported(src_format) || !supported(dst_format) || src_channels == 0 || dst_channels == 0
        || src_channels > 2 || dst_channels > 2 || src_rate <= 0 || dst_rate <= 0)
        return -1;
    *cvt = {};
    cvt->src_format = src_format;
    cvt->dst_format = dst_format;
    cvt->src_channels = src_channels;
    cvt->dst_channels = dst_channels;
    cvt->src_rate = src_rate;
    cvt->dst_rate = dst_rate;
    cvt->needed = (src_format != dst_format || src_channels != dst_channels || src_rate != dst_rate) ? 1 : 0;
    cvt->rate_incr = static_cast<double>(src_rate) / static_cast<double>(dst_rate);
    cvt->len_ratio = (static_cast<double>(bytesPerSample(dst_format)) / bytesPerSample(src_format))
        * (static_cast<double>(dst_channels) / src_channels) * (static_cast<double>(dst_rate) / src_rate);
    cvt->len_mult = static_cast<int>(cvt->len_ratio) + 1;
    if (cvt->len_mult < 1)
        cvt->len_mult = 1;
    return cvt->needed;
}

int SDL_ConvertAudio(SDL_AudioCVT* cvt)
{
    if (cvt == nullptr || cvt->buf == nullptr || cvt->len < 0)
        return -1;
    if (!cvt->needed)
    {
        cvt->len_cvt = cvt->len;
        return 0;
    }
    const int sbps = bytesPerSample(cvt->src_format);
    const int sch = cvt->src_channels;
    const int dch = cvt->dst_channels;
    const int dbps = bytesPerSample(cvt->dst_format);
    const size_t srcFrames = static_cast<size_t>(cvt->len) / static_cast<size_t>(sbps * sch);

    // Fast path: same rate and channel count, 16-bit signed with only the byte order differing (the common
    // case: little-endian WAV/CSS data into the big-endian mixer format) -- an in-place byte swap.
    if (cvt->src_rate == cvt->dst_rate && sch == dch
        && ((cvt->src_format == AUDIO_S16LSB && cvt->dst_format == AUDIO_S16MSB)
            || (cvt->src_format == AUDIO_S16MSB && cvt->dst_format == AUDIO_S16LSB)))
    {
        Uint8* p = cvt->buf;
        for (int i = 0; i + 1 < cvt->len; i += 2)
        {
            Uint8 t = p[i];
            p[i] = p[i + 1];
            p[i + 1] = t;
        }
        cvt->len_cvt = cvt->len & ~1;
        return 0;
    }

    // 1. decode to native int16 at the destination channel count
    std::vector<int16_t> pcm(srcFrames * dch);
    {
        const Uint8* p = cvt->buf;
        for (size_t i = 0; i < srcFrames; i++)
        {
            int16_t l = decode(cvt->src_format, p);
            int16_t r = sch == 2 ? decode(cvt->src_format, p + sbps) : l;
            p += sbps * sch;
            if (dch == 2)
            {
                pcm[i * 2] = l;
                pcm[i * 2 + 1] = r;
            }
            else
                pcm[i] = static_cast<int16_t>((static_cast<int>(l) + r) / 2);
        }
    }
    // 2. resample (linear) if the rates differ
    size_t dstFrames = srcFrames;
    if (cvt->src_rate != cvt->dst_rate && srcFrames > 0)
    {
        // 16.16 fixed-point linear interpolation: no floating point (this is a soft-float build).
        dstFrames = static_cast<size_t>((static_cast<uint64_t>(srcFrames) * cvt->dst_rate) / cvt->src_rate);
        std::vector<int16_t> out(dstFrames * dch);
        const uint32_t step = static_cast<uint32_t>((static_cast<uint64_t>(cvt->src_rate) << 16) / cvt->dst_rate);
        uint32_t pos = 0;
        for (size_t i = 0; i < dstFrames; i++, pos += step)
        {
            size_t idx = pos >> 16;
            int frac = static_cast<int>(pos & 0xFFFF);
            size_t nxt = idx + 1;
            if (nxt >= srcFrames)
            {
                nxt = srcFrames - 1;
                if (idx >= srcFrames)
                    idx = srcFrames - 1;
                frac = 0;
            }
            for (int c = 0; c < dch; c++)
            {
                int a = pcm[idx * dch + c];
                int b = pcm[nxt * dch + c];
                out[i * dch + c] = static_cast<int16_t>(a + (((b - a) * frac) >> 16));
            }
        }
        pcm.swap(out);
    }
    // 3. encode into cvt->buf (which the caller sized to len * len_mult)
    const size_t outBytes = dstFrames * dch * dbps;
    if (outBytes > static_cast<size_t>(cvt->len) * static_cast<size_t>(cvt->len_mult))
        return -1;
    Uint8* q = cvt->buf;
    for (size_t i = 0; i < dstFrames * dch; i++)
    {
        encode(cvt->dst_format, q, pcm[i]);
        q += dbps;
    }
    cvt->len_cvt = static_cast<int>(outBytes);
    return 0;
}

void SDL_MixAudioFormat(Uint8* dst, const Uint8* src, SDL_AudioFormat format, Uint32 len, int volume)
{
    if (volume <= 0 || len == 0)
        return;
    volume = std::min(volume, SDL_MIX_MAXVOLUME);
    switch (format)
    {
        case AUDIO_S16MSB:
        case AUDIO_S16LSB:
        {
            const bool be = format == AUDIO_S16MSB;
            for (Uint32 i = 0; i + 1 < len; i += 2)
            {
                int d = be ? static_cast<int16_t>((dst[i] << 8) | dst[i + 1])
                           : static_cast<int16_t>(dst[i] | (dst[i + 1] << 8));
                int s = be ? static_cast<int16_t>((src[i] << 8) | src[i + 1])
                           : static_cast<int16_t>(src[i] | (src[i + 1] << 8));
                int v = d + ((s * volume) / SDL_MIX_MAXVOLUME);
                v = std::clamp(v, -32768, 32767);
                if (be)
                {
                    dst[i] = static_cast<Uint8>((v >> 8) & 0xFF);
                    dst[i + 1] = static_cast<Uint8>(v & 0xFF);
                }
                else
                {
                    dst[i] = static_cast<Uint8>(v & 0xFF);
                    dst[i + 1] = static_cast<Uint8>((v >> 8) & 0xFF);
                }
            }
            break;
        }
        case AUDIO_U8:
            for (Uint32 i = 0; i < len; i++)
            {
                int v = (static_cast<int>(dst[i]) - 128) + (((static_cast<int>(src[i]) - 128) * volume) / SDL_MIX_MAXVOLUME);
                dst[i] = static_cast<Uint8>(std::clamp(v, -128, 127) + 128);
            }
            break;
        case AUDIO_S8:
            for (Uint32 i = 0; i < len; i++)
            {
                int v = static_cast<int8_t>(dst[i]) + ((static_cast<int8_t>(src[i]) * volume) / SDL_MIX_MAXVOLUME);
                dst[i] = static_cast<Uint8>(std::clamp(v, -128, 127));
            }
            break;
        default:
            break;
    }
}

// ---- rwops -----------------------------------------------------------------------------------
void* SDL_malloc(size_t size)
{
    return std::malloc(size);
}
SDL_RWops* SDL_AllocRW(void)
{
    auto* rw = static_cast<SDL_RWops*>(std::malloc(sizeof(SDL_RWops)));
    if (rw != nullptr)
        std::memset(rw, 0, sizeof(SDL_RWops));
    return rw;
}
void SDL_FreeRW(SDL_RWops* area)
{
    std::free(area);
}
Sint64 SDL_RWsize(SDL_RWops* c)
{
    return (c != nullptr && c->size != nullptr) ? c->size(c) : -1;
}
Sint64 SDL_RWseek(SDL_RWops* c, Sint64 offset, int whence)
{
    return (c != nullptr && c->seek != nullptr) ? c->seek(c, offset, whence) : -1;
}
Sint64 SDL_RWtell(SDL_RWops* c)
{
    return SDL_RWseek(c, 0, RW_SEEK_CUR);
}
size_t SDL_RWread(SDL_RWops* c, void* ptr, size_t size, size_t maxnum)
{
    return (c != nullptr && c->read != nullptr) ? c->read(c, ptr, size, maxnum) : 0;
}
size_t SDL_RWwrite(SDL_RWops* c, const void* ptr, size_t size, size_t num)
{
    return (c != nullptr && c->write != nullptr) ? c->write(c, ptr, size, num) : 0;
}
int SDL_RWclose(SDL_RWops* c)
{
    return (c != nullptr && c->close != nullptr) ? c->close(c) : -1;
}
Uint16 SDL_ReadLE16(SDL_RWops* src)
{
    Uint8 b[2] = { 0, 0 };
    SDL_RWread(src, b, 1, 2);
    return static_cast<Uint16>(b[0] | (b[1] << 8));
}
Uint32 SDL_ReadLE32(SDL_RWops* src)
{
    Uint8 b[4] = { 0, 0, 0, 0 };
    SDL_RWread(src, b, 1, 4);
    return static_cast<Uint32>(b[0]) | (static_cast<Uint32>(b[1]) << 8) | (static_cast<Uint32>(b[2]) << 16)
        | (static_cast<Uint32>(b[3]) << 24);
}
}
#endif
