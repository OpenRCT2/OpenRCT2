#pragma once
// Stage-probe tracing for the AmigaOS port: one line per call, written straight to disk.
#ifdef __amigaos__
extern "C" void amiga_trace(const char* line);
    #define AMIGA_TRACE(msg) amiga_trace(msg)
    #define AMIGA_TRACE_ONCE(msg)                                                                                              \
        do                                                                                                                     \
        {                                                                                                                      \
            static bool _traced = false;                                                                                       \
            if (!_traced)                                                                                                      \
            {                                                                                                                  \
                _traced = true;                                                                                                \
                amiga_trace(msg);                                                                                              \
            }                                                                                                                  \
        } while (0)
#elif defined(OPENRCT2_TRACE_STDERR)
    #include <cstdio>
    #define AMIGA_TRACE(msg) std::fprintf(stderr, "TRACE: %s\n", msg)
    #define AMIGA_TRACE_ONCE(msg)                                                                                              \
        do                                                                                                                     \
        {                                                                                                                      \
            static bool _traced = false;                                                                                       \
            if (!_traced)                                                                                                      \
            {                                                                                                                  \
                _traced = true;                                                                                                \
                std::fprintf(stderr, "TRACE: %s\n", msg);                                                                      \
            }                                                                                                                  \
        } while (0)
#else
    #define AMIGA_TRACE(msg) ((void)0)
    #define AMIGA_TRACE_ONCE(msg) ((void)0)
#endif
