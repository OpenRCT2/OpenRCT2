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

#ifndef DISABLE_OPENGL

#define NO_EXTERN_GLAPI
#include "OpenGLAPI.h"

#if OPENGL_NO_LINK

#include <SDL_video.h>

#include "../../core/Console.hpp"

template <typename T>
static inline bool SetProc(T * func, const char * name)
{
    T address = (T)SDL_GL_GetProcAddress(name);
    if (address == nullptr)
    {
        return false;
    }

    *func = address;
    return true;
}

#define SetupOpenGLFunction(func)           \
    {                                       \
        if (!SetProc(&func, "" #func ""))   \
        {                                   \
            return "" #func "";             \
        }                                   \
    }

static const char * TryLoadAllProcAddresses()
{
    // 1.1 functions
    SetupOpenGLFunction(glBegin);
    SetupOpenGLFunction(glBindTexture);
    SetupOpenGLFunction(glBlendFunc);
    SetupOpenGLFunction(glClear);
    SetupOpenGLFunction(glClearColor);
    SetupOpenGLFunction(glColor3f);
    SetupOpenGLFunction(glColor4f);
    SetupOpenGLFunction(glDeleteTextures);
    SetupOpenGLFunction(glDisable);
    SetupOpenGLFunction(glEnable);
    SetupOpenGLFunction(glEnd);
    SetupOpenGLFunction(glGenTextures);
    SetupOpenGLFunction(glLoadIdentity);
    SetupOpenGLFunction(glMatrixMode);
    SetupOpenGLFunction(glOrtho);
    SetupOpenGLFunction(glScalef);
    SetupOpenGLFunction(glTexCoord2f);
    SetupOpenGLFunction(glTexImage2D);
    SetupOpenGLFunction(glTexParameteri);
    SetupOpenGLFunction(glTranslatef);
    SetupOpenGLFunction(glVertex2i);
    SetupOpenGLFunction(glViewport);

    // 2.0+ functions
    SetupOpenGLFunction(glCreateShader);
    SetupOpenGLFunction(glDeleteShader);

    return nullptr;
}

#endif /* #if OPENGL_NO_LINK */

bool OpenGLAPI::Initialise()
{
#ifdef OPENGL_NO_LINK
    const char * failedProcName = TryLoadAllProcAddresses();
    if (failedProcName != nullptr)
    {
        Console::Error::WriteFormat("Failed to load %s.\n", failedProcName);
        return false;
    }
#endif
    return true;
}

#endif /* DISABLE_OPENGL */
