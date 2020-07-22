/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

#    include "OpenGLAPI.h"

#    if OPENGL_NO_LINK

#        define OPENGL_PROC(TYPE, PROC) TYPE PROC = nullptr;
#        include "OpenGLAPIProc.h"
#        undef OPENGL_PROC

#        include <SDL_video.h>
#        include <openrct2/core/Console.hpp>

static const char* TryLoadAllProcAddresses()
{
#        define OPENGL_PROC(TYPE, PROC)                                                                                        \
            {                                                                                                                  \
                PROC = reinterpret_cast<TYPE>(SDL_GL_GetProcAddress(#PROC));                                                   \
                if (PROC == nullptr)                                                                                           \
                {                                                                                                              \
                    return #PROC;                                                                                              \
                }                                                                                                              \
            }
#        include "OpenGLAPIProc.h"
#        undef OPENGL_PROC

    return nullptr;
}

#    endif /* #if OPENGL_NO_LINK */

namespace OpenGLState
{
    uint16_t ActiveTexture;
    GLuint CurrentProgram;

    void Reset()
    {
        ActiveTexture = UINT16_MAX;
        CurrentProgram = UINT32_MAX;
    }
} // namespace OpenGLState

void OpenGLAPI::SetTexture(uint16_t index, GLenum type, GLuint texture)
{
    if (OpenGLState::ActiveTexture != index)
    {
        glActiveTexture(GL_TEXTURE0 + index);
    }
    glBindTexture(type, texture);
}

bool OpenGLAPI::Initialise()
{
    OpenGLState::Reset();

#    ifdef OPENGL_NO_LINK
    const char* failedProcName = TryLoadAllProcAddresses();
    if (failedProcName != nullptr)
    {
        Console::Error::WriteLine("Failed to load %s.", failedProcName);
        return false;
    }
#    endif
    return true;
}

#endif /* DISABLE_OPENGL */
