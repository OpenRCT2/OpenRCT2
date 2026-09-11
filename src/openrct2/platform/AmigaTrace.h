#pragma once
// Stage-probe tracing for the AmigaOS port: one line per call, written straight to disk.
#ifdef __amigaos__
extern "C" void amiga_trace(const char* line);
    #define AMIGA_TRACE(msg) amiga_trace(msg)
#else
    #define AMIGA_TRACE(msg) ((void)0)
#endif
