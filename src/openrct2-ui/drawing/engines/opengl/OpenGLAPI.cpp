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

#define NO_EXTERN_GLAPI
#include "OpenGLAPI.h"

#if OPENGL_NO_LINK

#include <SDL_video.h>

#include <openrct2/core/Console.hpp>

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
    SetupOpenGLFunction(glActiveTexture);
    SetupOpenGLFunction(glBegin);
    SetupOpenGLFunction(glBindTexture);
    SetupOpenGLFunction(glBlendFunc);
    SetupOpenGLFunction(glClear);
    SetupOpenGLFunction(glClearColor);
    SetupOpenGLFunction(glCullFace);
    SetupOpenGLFunction(glDeleteTextures);
    SetupOpenGLFunction(glDisable);
    SetupOpenGLFunction(glDrawArrays);
    SetupOpenGLFunction(glEnable);
    SetupOpenGLFunction(glEnd);
    SetupOpenGLFunction(glGenTextures);
    SetupOpenGLFunction(glGetError);
    SetupOpenGLFunction(glPixelStorei);
    SetupOpenGLFunction(glReadPixels);
    SetupOpenGLFunction(glTexImage2D);
    SetupOpenGLFunction(glTexParameteri);
    SetupOpenGLFunction(glViewport);
    SetupOpenGLFunction(glTexSubImage3D);
    SetupOpenGLFunction(glTexImage3D);
    SetupOpenGLFunction(glGetIntegerv);
    SetupOpenGLFunction(glGetTexImage);

    // 2.0+ functions
    SetupOpenGLFunction(glAttachShader);
    SetupOpenGLFunction(glBindBuffer);
    SetupOpenGLFunction(glBindFragDataLocation);
    SetupOpenGLFunction(glBindFramebuffer);
    SetupOpenGLFunction(glBindVertexArray);
    SetupOpenGLFunction(glBufferData);
    SetupOpenGLFunction(glCompileShader);
    SetupOpenGLFunction(glCreateProgram);
    SetupOpenGLFunction(glCreateShader);
    SetupOpenGLFunction(glDeleteBuffers);
    SetupOpenGLFunction(glDeleteFramebuffers);
    SetupOpenGLFunction(glDeleteProgram);
    SetupOpenGLFunction(glDeleteShader);
    SetupOpenGLFunction(glDeleteVertexArrays);
    SetupOpenGLFunction(glDetachShader);
    SetupOpenGLFunction(glEnableVertexAttribArray);
    SetupOpenGLFunction(glFramebufferTexture2D);
    SetupOpenGLFunction(glGetAttribLocation);
    SetupOpenGLFunction(glGenBuffers);
    SetupOpenGLFunction(glGenFramebuffers);
    SetupOpenGLFunction(glGetProgramInfoLog);
    SetupOpenGLFunction(glGetProgramiv);
    SetupOpenGLFunction(glGetShaderInfoLog);
    SetupOpenGLFunction(glGetShaderiv);
    SetupOpenGLFunction(glGetUniformLocation);
    SetupOpenGLFunction(glGenVertexArrays);
    SetupOpenGLFunction(glLinkProgram);
    SetupOpenGLFunction(glShaderSource);
    SetupOpenGLFunction(glUniform1i);
    SetupOpenGLFunction(glUniform1iv);
    SetupOpenGLFunction(glUniform2i);
    SetupOpenGLFunction(glUniform2f);
    SetupOpenGLFunction(glUniform4f);
    SetupOpenGLFunction(glUniform4i);
    SetupOpenGLFunction(glUniform4fv);
    SetupOpenGLFunction(glUseProgram);
    SetupOpenGLFunction(glVertexAttribIPointer);
    SetupOpenGLFunction(glVertexAttribPointer);
    SetupOpenGLFunction(glDrawArraysInstanced);
    SetupOpenGLFunction(glVertexAttribDivisor);
    SetupOpenGLFunction(glBlendFuncSeparate);

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
