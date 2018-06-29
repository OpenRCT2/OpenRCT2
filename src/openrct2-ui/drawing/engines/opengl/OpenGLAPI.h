/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>

#ifdef OPENGL_NO_LINK

// BEGIN [Do not define 1.1 function signatures]
#define glActiveTexture     __static__glActiveTexture
#define glBegin             __static__glBegin
#define glBindTexture       __static__glBindTexture
#define glBlendFunc         __static__glBlendFunc
#define glClear             __static__glClear
#define glClearColor        __static__glClearColor
#define glCullFace          __static__glCullFace
#define glDeleteTextures    __static__glDeleteTextures
#define glDepthFunc         __static__glDepthFunc
#define glDisable           __static__glDisable
#define glDrawArrays        __static__glDrawArrays
#define glEnable            __static__glEnable
#define glEnd               __static__glEnd
#define glGenTextures       __static__glGenTextures
#define glGetError          __static__glGetError
#define glPixelStorei       __static__glPixelStorei
#define glReadPixels        __static__glReadPixels
#define glTexImage2D        __static__glTexImage2D
#define glTexParameteri     __static__glTexParameteri
#define glViewport          __static__glViewport
#define glTexSubImage3D     __static__glTexSubImage3D
#define glTexImage3D        __static__glTexImage3D
#define glGetIntegerv       __static__glGetIntegerv
#define glGetTexImage       __static__glGetTexImage

#endif

#include <SDL2/SDL_opengl.h>

#ifdef OPENGL_NO_LINK

// END [Do not define 1.1 function signatures]
#undef glActiveTexture
#undef glBegin
#undef glBindTexture
#undef glBlendFunc
#undef glClear
#undef glClearColor
#undef glCullFace
#undef glDeleteTextures
#undef glDepthFunc
#undef glDisable
#undef glDrawArrays
#undef glEnable
#undef glEnd
#undef glGenTextures
#undef glGetError
#undef glPixelStorei
#undef glReadPixels
#undef glTexImage2D
#undef glTexParameteri
#undef glViewport
#undef glTexSubImage3D
#undef glTexImage3D
#undef glGetIntegerv
#undef glGetTexImage

// 1.1 function signatures
typedef void   (APIENTRYP PFNGLBEGINPROC         )(GLenum mode);
typedef void   (APIENTRYP PFNGLBINDTEXTUREPROC   )(GLenum target, GLuint texture);
typedef void   (APIENTRYP PFNGLBLENDFUNCPROC     )(GLenum sfactor, GLenum dfactor);
typedef void   (APIENTRYP PFNGLCLEARPROC         )(GLbitfield mask);
typedef void   (APIENTRYP PFNGLCLEARCOLORPROC    )(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef void   (APIENTRYP PFNGLCULLFACEPROC      )(GLenum mode);
typedef void   (APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint *textures);
typedef void   (APIENTRYP PFNGLDEPTHFUNCPROC     )(GLenum func);
typedef void   (APIENTRYP PFNGLDISABLEPROC       )(GLenum cap);
typedef void   (APIENTRYP PFNGLDRAWARRAYSPROC    )(GLenum mode, GLint first, GLsizei count);
typedef void   (APIENTRYP PFNGLENABLEPROC        )(GLenum cap);
typedef void   (APIENTRYP PFNGLENDPROC           )(void);
typedef GLenum (APIENTRYP PFNGLGETERRORPROC      )(void);
typedef void   (APIENTRYP PFNGLGENTEXTURESPROC   )(GLsizei n, GLuint *textures);
typedef void   (APIENTRYP PFNGLPIXELSTOREIPROC   )(GLenum pname, GLint param);
typedef void   (APIENTRYP PFNGLREADPIXELSPROC    )(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * pixels);
typedef void   (APIENTRYP PFNGLTEXIMAGE2DPROC    )(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void   (APIENTRYP PFNGLTEXPARAMETERIPROC )(GLenum target, GLenum pname, GLint param);
typedef void   (APIENTRYP PFNGLVIEWPORTPROC      )(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void   (APIENTRYP PFNGLTEXSUBIMAGE3DPROC )(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* data);
typedef void   (APIENTRYP PFNGLTEXIMAGE3DPROC    )(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * data);
typedef void   (APIENTRYP PFNGLGETINTERGERVPROC  )(GLenum pname, GLint * data);
typedef void   (APIENTRYP PFNGLGETTEXIMAGEPROC   )(GLenum target, GLint level, GLenum format, GLenum type, GLvoid * img);

#define OPENGL_PROC(TYPE, PROC) extern TYPE PROC;
#include "OpenGLAPIProc.h"
#undef OPENGL_PROC

#endif /* OPENGL_NO_LINK */

inline void CheckGLError()
{
    GLenum error = glGetError();
    while (error != GL_NO_ERROR)
    {
        log_error("OpenGL Error 0x%04X", error);
        error = glGetError();
    }
}

namespace OpenGLAPI
{
    bool Initialise();
    void SetTexture(uint16_t index, GLenum type, GLuint texture);
}

namespace OpenGLState
{
    extern uint16_t ActiveTexture;
    extern GLuint CurrentProgram;

    void Reset();
}
