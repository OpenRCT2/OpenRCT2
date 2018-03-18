#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#ifndef DISABLE_OPENGL

#include "OpenGLAPI.h"

#if OPENGL_NO_LINK

#define OPENGL_PROC(TYPE, PROC) TYPE PROC = nullptr;
#include "OpenGLAPIProc.h"
#undef OPENGL_PROC

#include <SDL2/SDL_video.h>

#include <openrct2/core/Console.hpp>

static const char * TryLoadAllProcAddresses()
{
#define OPENGL_PROC(TYPE, PROC)                     \
    {                                               \
        PROC = (TYPE)SDL_GL_GetProcAddress(#PROC);  \
        if (PROC == nullptr)                        \
        {                                           \
            return #PROC;                           \
        }                                           \
    }
#include "OpenGLAPIProc.h"
#undef OPENGL_PROC

    return nullptr;
}

#endif /* #if OPENGL_NO_LINK */

namespace OpenGLState
{
    uint16 ActiveTexture;
    GLuint CurrentProgram;

    void Reset()
    {
        ActiveTexture = UINT16_MAX;
        CurrentProgram = UINT32_MAX;
    }
}

void OpenGLAPI::SetTexture(uint16 index, GLenum type, GLuint texture)
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

#ifdef OPENGL_NO_LINK
    const char * failedProcName = TryLoadAllProcAddresses();
    if (failedProcName != nullptr)
    {
        Console::Error::WriteLine("Failed to load %s.", failedProcName);
        return false;
    }
#endif
    return true;
}

#endif /* DISABLE_OPENGL */
