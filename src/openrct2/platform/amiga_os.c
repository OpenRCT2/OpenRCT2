/*****************************************************************************
 * AmigaOS bindings kept in plain C so the NDK headers never meet the C++ tree.
 *****************************************************************************/
#ifdef __amigaos__

    #include <proto/dos.h>
    #include <proto/exec.h>
    #include <proto/timer.h>
    #include <devices/timer.h>
    #include <dos/dosextens.h>
    #include <string.h>

/* libnix reads this at startup and swaps to a stack of this size; the CLI default is 4 KB. */
unsigned long __stack = 8UL * 1024UL * 1024UL;

void amiga_sleep_ms(unsigned ms)
{
    /* Delay() works in 1/50 s ticks; never busy-wait, never pass 0 for a non-zero request. */
    ULONG ticks = (ms + 19) / 20;
    if (ticks == 0 && ms != 0)
        ticks = 1;
    if (ticks != 0)
        Delay(ticks);
}

unsigned amiga_ticks_ms(void)
{
    /* GetSysTime() is wall-clock with microsecond resolution; good enough for frame timing. */
    struct Library* TimerBase;
    static struct MsgPort* port = NULL;
    static struct timerequest* req = NULL;
    struct timeval tv;
    if (req == NULL)
    {
        port = CreateMsgPort();
        if (port == NULL)
            return 0;
        req = (struct timerequest*)CreateIORequest(port, sizeof(struct timerequest));
        if (req == NULL)
            return 0;
        if (OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest*)req, 0) != 0)
        {
            req = NULL;
            return 0;
        }
    }
    TimerBase = (struct Library*)req->tr_node.io_Device;
    GetSysTime(&tv);
    return (unsigned)(tv.tv_secs * 1000u + tv.tv_micro / 1000u);
}

/* Full path of the running program, e.g. "Work:OpenRCT2/openrct2-cli". Returns 0 on failure. */
int amiga_program_path(char* buf, unsigned size)
{
    BPTR dir = GetProgramDir();
    char name[108];
    if (size == 0)
        return 0;
    buf[0] = '\0';
    if (dir != 0)
    {
        if (!NameFromLock(dir, buf, size))
            buf[0] = '\0';
    }
    if (!GetProgramName(name, sizeof(name)))
        name[0] = '\0';
    if (buf[0] == '\0')
        return 0;
    if (!AddPart(buf, name, size))
        return 0;
    return 1;
}

/* Append one line to a trace file, opening and closing it each time so it survives a wedged process.
 * Opt-in: the trace is written only when the environment variable OPENRCT2_TRACE names the file
 * (e.g. `SetEnv OPENRCT2_TRACE Work:OpenRCT2/trace.txt`). Off by default so a tester's build does not
 * pay thousands of Open() calls during the park load, nor leave a growing log behind. */
void amiga_trace(const char* line)
{
    static unsigned t0 = 0;
    static int enabled = -1;
    static char path[256];
    char stamp[24];
    unsigned now;
    BPTR fh;
    if (enabled < 0)
    {
        enabled = (GetVar((STRPTR) "OPENRCT2_TRACE", (STRPTR)path, sizeof(path), 0) > 0) ? 1 : 0;
    }
    if (!enabled)
        return;
    now = amiga_ticks_ms();
    if (t0 == 0)
        t0 = now;
    fh = Open((STRPTR)path, MODE_READWRITE);
    if (fh == 0)
        return;
    Seek(fh, 0, OFFSET_END);
    /* elapsed milliseconds since the first trace line, so the load can be profiled from the file */
    {
        unsigned ms = now - t0;
        int n = 0;
        char rev[12];
        stamp[n++] = '[';
        stamp[n++] = '+';
        {
            int r = 0;
            do { rev[r++] = (char)('0' + ms % 10); ms /= 10; } while (ms != 0);
            while (r > 0) stamp[n++] = rev[--r];
        }
        stamp[n++] = 'm'; stamp[n++] = 's'; stamp[n++] = ']'; stamp[n++] = ' ';
        Write(fh, (APTR)stamp, n);
    }
    Write(fh, (APTR)line, strlen(line));
    Write(fh, (APTR)"\n", 1);
    Close(fh);
}

#endif
