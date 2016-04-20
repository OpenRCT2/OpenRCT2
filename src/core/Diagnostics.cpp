#include <SDL_platform.h>

#if defined(DEBUG) && defined(__WINDOWS__)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include "Diagnostics.hpp"

namespace Debug
{
    void Break()
    {
#if DEBUG
#if __WINDOWS__
        if (IsDebuggerPresent()) {
            DebugBreak();
        }
#endif
#endif
    }
}
