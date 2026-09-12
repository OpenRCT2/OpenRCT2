/* See amiga_audio.h. */
#ifdef __amigaos__
    #include "amiga_audio.h"

    #include <devices/ahi.h>
    #include <exec/memory.h>
    #include <proto/dos.h>
    #include <proto/exec.h>
    #include <string.h>

    #define MAX_BUFFERS 4

static struct MsgPort* g_port = NULL;
static struct AHIRequest* g_req[MAX_BUFFERS];
static unsigned char* g_buf[MAX_BUFFERS];
static int g_inflight[MAX_BUFFERS]; /* 1 while SendIO'd and not yet reaped */
static int g_num = 0;
static int g_bytes = 0;
static int g_freq = 0;
static int g_open = 0;
static struct AHIRequest* g_last = NULL; /* most recently queued request, for ahir_Link */
static unsigned g_written = 0, g_underruns = 0;

static void reap(int i)
{
    if (g_inflight[i] && CheckIO((struct IORequest*)g_req[i]))
    {
        WaitIO((struct IORequest*)g_req[i]); /* completed: this just removes the reply */
        g_inflight[i] = 0;
        if (g_last == g_req[i])
            g_last = NULL;
    }
}

int amiga_audio_available(void)
{
    struct MsgPort* port = CreateMsgPort();
    struct AHIRequest* req;
    int ok = 0;
    if (port == NULL)
        return 0;
    req = (struct AHIRequest*)CreateIORequest(port, sizeof(struct AHIRequest));
    if (req != NULL)
    {
        req->ahir_Version = 4;
        if (OpenDevice((STRPTR)AHINAME, AHI_DEFAULT_UNIT, (struct IORequest*)req, 0) == 0)
        {
            CloseDevice((struct IORequest*)req);
            ok = 1;
        }
        DeleteIORequest((struct IORequest*)req);
    }
    DeleteMsgPort(port);
    return ok;
}

int amiga_audio_open(int freq, int frames, int numBuffers)
{
    int i;
    amiga_audio_close();
    if (numBuffers < 2)
        numBuffers = 2;
    if (numBuffers > MAX_BUFFERS)
        numBuffers = MAX_BUFFERS;
    g_port = CreateMsgPort();
    if (g_port == NULL)
        return 0;
    g_req[0] = (struct AHIRequest*)CreateIORequest(g_port, sizeof(struct AHIRequest));
    if (g_req[0] == NULL)
    {
        amiga_audio_close();
        return 0;
    }
    g_req[0]->ahir_Version = 4;
    if (OpenDevice((STRPTR)AHINAME, AHI_DEFAULT_UNIT, (struct IORequest*)g_req[0], 0) != 0)
    {
        DeleteIORequest((struct IORequest*)g_req[0]);
        g_req[0] = NULL;
        amiga_audio_close();
        return 0;
    }
    g_open = 1;
    g_num = numBuffers;
    g_bytes = frames * 4; /* 16-bit stereo */
    g_freq = freq;
    for (i = 0; i < g_num; i++)
    {
        if (i > 0)
        {
            g_req[i] = (struct AHIRequest*)CreateIORequest(g_port, sizeof(struct AHIRequest));
            if (g_req[i] == NULL)
            {
                amiga_audio_close();
                return 0;
            }
            /* Clone the opened request so every copy refers to the same device/unit. */
            memcpy(g_req[i], g_req[0], sizeof(struct AHIRequest));
        }
        g_buf[i] = (unsigned char*)AllocVec(g_bytes, MEMF_PUBLIC | MEMF_CLEAR);
        if (g_buf[i] == NULL)
        {
            amiga_audio_close();
            return 0;
        }
        g_inflight[i] = 0;
    }
    g_last = NULL;
    g_written = g_underruns = 0;
    return 1;
}

int amiga_audio_pump(amiga_audio_fill_fn fill, void* user)
{
    int i, filled = 0, busy = 0;
    if (!g_open)
        return 0;
    for (i = 0; i < g_num; i++)
        reap(i);
    for (i = 0; i < g_num; i++)
        if (g_inflight[i])
            busy++;
    if (busy == 0 && g_written > 0)
        g_underruns++;
    for (i = 0; i < g_num; i++)
    {
        struct AHIRequest* r = g_req[i];
        if (g_inflight[i])
            continue;
        fill(user, g_buf[i], g_bytes);
        r->ahir_Std.io_Message.mn_Node.ln_Pri = 0;
        r->ahir_Std.io_Command = CMD_WRITE;
        r->ahir_Std.io_Data = g_buf[i];
        r->ahir_Std.io_Length = g_bytes;
        r->ahir_Std.io_Offset = 0;
        r->ahir_Type = AHIST_S16S;
        r->ahir_Frequency = g_freq;
        r->ahir_Volume = 0x10000;  /* 1.0 */
        r->ahir_Position = 0x8000; /* centre */
        r->ahir_Link = g_last;     /* gapless: play after the previous request */
        SendIO((struct IORequest*)r);
        g_inflight[i] = 1;
        g_last = r;
        g_written++;
        filled++;
    }
    return filled;
}

void amiga_audio_close(void)
{
    int i;
    for (i = 0; i < MAX_BUFFERS; i++)
    {
        if (g_req[i] != NULL && g_inflight[i])
        {
            AbortIO((struct IORequest*)g_req[i]);
            WaitIO((struct IORequest*)g_req[i]);
            g_inflight[i] = 0;
        }
    }
    if (g_open && g_req[0] != NULL)
        CloseDevice((struct IORequest*)g_req[0]);
    g_open = 0;
    for (i = 0; i < MAX_BUFFERS; i++)
    {
        if (g_req[i] != NULL)
        {
            DeleteIORequest((struct IORequest*)g_req[i]);
            g_req[i] = NULL;
        }
        if (g_buf[i] != NULL)
        {
            FreeVec(g_buf[i]);
            g_buf[i] = NULL;
        }
    }
    if (g_port != NULL)
    {
        DeleteMsgPort(g_port);
        g_port = NULL;
    }
    g_last = NULL;
    g_num = 0;
}

int amiga_audio_getenv(const char* name, char* buf, int len)
{
    return GetVar((STRPTR)name, (STRPTR)buf, len, 0) > 0 ? 1 : 0;
}

void amiga_audio_stats(unsigned* written, unsigned* underruns)
{
    if (written)
        *written = g_written;
    if (underruns)
        *underruns = g_underruns;
}
#endif
