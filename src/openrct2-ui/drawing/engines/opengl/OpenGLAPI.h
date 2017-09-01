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

#pragma once

#include <openrct2/common.h>

#if OPENGL_NO_LINK

// BEGIN [Do not define 1.1 function signatures]
#define glActiveTexture     __static__glActiveTexture
#define glBegin             __static__glBegin
#define glBindTexture       __static__glBindTexture
#define glBlendFunc         __static__glBlendFunc
#define glClear             __static__glClear
#define glClearColor        __static__glClearColor
#define glCullFace          __static__glCullFace
#define glDeleteTextures    __static__glDeleteTextures
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

#include <SDL_opengl.h>

#if OPENGL_NO_LINK

// END [Do not define 1.1 function signatures]
#undef glActiveTexture
#undef glBegin
#undef glBindTexture
#undef glBlendFunc
#undef glClear
#undef glClearColor
#undef glCullFace
#undef glDeleteTextures
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

#ifdef NO_EXTERN_GLAPI
    // Defines the function pointers
    #define GLAPI_DECL
    #define GLAPI_SET   = nullptr
#else
    // Defines the functions as "extern"
    #define GLAPI_DECL  extern
    #define GLAPI_SET
#endif

// 1.1 function pointers
GLAPI_DECL PFNGLACTIVETEXTUREPROC               glActiveTexture             GLAPI_SET;
GLAPI_DECL PFNGLBEGINPROC                       glBegin                     GLAPI_SET;
GLAPI_DECL PFNGLBINDTEXTUREPROC                 glBindTexture               GLAPI_SET;
GLAPI_DECL PFNGLBLENDFUNCPROC                   glBlendFunc                 GLAPI_SET;
GLAPI_DECL PFNGLCLEARPROC                       glClear                     GLAPI_SET;
GLAPI_DECL PFNGLCLEARCOLORPROC                  glClearColor                GLAPI_SET;
GLAPI_DECL PFNGLCULLFACEPROC                    glCullFace                  GLAPI_SET;
GLAPI_DECL PFNGLDELETETEXTURESPROC              glDeleteTextures            GLAPI_SET;
GLAPI_DECL PFNGLDISABLEPROC                     glDisable                   GLAPI_SET;
GLAPI_DECL PFNGLDRAWARRAYSPROC                  glDrawArrays                GLAPI_SET;
GLAPI_DECL PFNGLENABLEPROC                      glEnable                    GLAPI_SET;
GLAPI_DECL PFNGLENDPROC                         glEnd                       GLAPI_SET;
GLAPI_DECL PFNGLGENTEXTURESPROC                 glGenTextures               GLAPI_SET;
GLAPI_DECL PFNGLGETERRORPROC                    glGetError                  GLAPI_SET;
GLAPI_DECL PFNGLPIXELSTOREIPROC                 glPixelStorei               GLAPI_SET;
GLAPI_DECL PFNGLREADPIXELSPROC                  glReadPixels                GLAPI_SET;
GLAPI_DECL PFNGLTEXIMAGE2DPROC                  glTexImage2D                GLAPI_SET;
GLAPI_DECL PFNGLTEXPARAMETERIPROC               glTexParameteri             GLAPI_SET;
GLAPI_DECL PFNGLVIEWPORTPROC                    glViewport                  GLAPI_SET;
GLAPI_DECL PFNGLTEXSUBIMAGE3DPROC               glTexSubImage3D             GLAPI_SET;
GLAPI_DECL PFNGLTEXIMAGE3DPROC                  glTexImage3D                GLAPI_SET;
GLAPI_DECL PFNGLGETINTERGERVPROC                glGetIntegerv               GLAPI_SET;
GLAPI_DECL PFNGLGETTEXIMAGEPROC                 glGetTexImage               GLAPI_SET;

// 2.0+ function pointers
GLAPI_DECL PFNGLATTACHSHADERPROC                glAttachShader              GLAPI_SET;
GLAPI_DECL PFNGLBINDBUFFERPROC                  glBindBuffer                GLAPI_SET;
GLAPI_DECL PFNGLBINDFRAGDATALOCATIONPROC        glBindFragDataLocation      GLAPI_SET;
GLAPI_DECL PFNGLBINDFRAMEBUFFERPROC             glBindFramebuffer           GLAPI_SET;
GLAPI_DECL PFNGLBINDVERTEXARRAYPROC             glBindVertexArray           GLAPI_SET;
GLAPI_DECL PFNGLBUFFERDATAPROC                  glBufferData                GLAPI_SET;
GLAPI_DECL PFNGLCOMPILESHADERPROC               glCompileShader             GLAPI_SET;
GLAPI_DECL PFNGLCREATEPROGRAMPROC               glCreateProgram             GLAPI_SET;
GLAPI_DECL PFNGLCREATESHADERPROC                glCreateShader              GLAPI_SET;
GLAPI_DECL PFNGLDELETEBUFFERSPROC               glDeleteBuffers             GLAPI_SET;
GLAPI_DECL PFNGLDELETEFRAMEBUFFERSPROC          glDeleteFramebuffers        GLAPI_SET;
GLAPI_DECL PFNGLDELETEPROGRAMPROC               glDeleteProgram             GLAPI_SET;
GLAPI_DECL PFNGLDELETESHADERPROC                glDeleteShader              GLAPI_SET;
GLAPI_DECL PFNGLDELETEVERTEXARRAYSPROC          glDeleteVertexArrays        GLAPI_SET;
GLAPI_DECL PFNGLDETACHSHADERPROC                glDetachShader              GLAPI_SET;
GLAPI_DECL PFNGLENABLEVERTEXATTRIBARRAYPROC     glEnableVertexAttribArray   GLAPI_SET;
GLAPI_DECL PFNGLFRAMEBUFFERTEXTURE2DPROC        glFramebufferTexture2D      GLAPI_SET;
GLAPI_DECL PFNGLGETATTRIBLOCATIONPROC           glGetAttribLocation         GLAPI_SET;
GLAPI_DECL PFNGLGENBUFFERSPROC                  glGenBuffers                GLAPI_SET;
GLAPI_DECL PFNGLGENFRAMEBUFFERSPROC             glGenFramebuffers           GLAPI_SET;
GLAPI_DECL PFNGLGETPROGRAMINFOLOGPROC           glGetProgramInfoLog         GLAPI_SET;
GLAPI_DECL PFNGLGETPROGRAMIVPROC                glGetProgramiv              GLAPI_SET;
GLAPI_DECL PFNGLGETSHADERINFOLOGPROC            glGetShaderInfoLog          GLAPI_SET;
GLAPI_DECL PFNGLGETSHADERIVPROC                 glGetShaderiv               GLAPI_SET;
GLAPI_DECL PFNGLGETUNIFORMLOCATIONPROC          glGetUniformLocation        GLAPI_SET;
GLAPI_DECL PFNGLGENVERTEXARRAYSPROC             glGenVertexArrays           GLAPI_SET;
GLAPI_DECL PFNGLLINKPROGRAMPROC                 glLinkProgram               GLAPI_SET;
GLAPI_DECL PFNGLSHADERSOURCEPROC                glShaderSource              GLAPI_SET;
GLAPI_DECL PFNGLUNIFORM1IPROC                   glUniform1i                 GLAPI_SET;
GLAPI_DECL PFNGLUNIFORM1IVPROC                  glUniform1iv                GLAPI_SET;
GLAPI_DECL PFNGLUNIFORM2IPROC                   glUniform2i                 GLAPI_SET;
GLAPI_DECL PFNGLUNIFORM2FPROC                   glUniform2f                 GLAPI_SET;
GLAPI_DECL PFNGLUNIFORM4FPROC                   glUniform4f                 GLAPI_SET;
GLAPI_DECL PFNGLUNIFORM4IPROC                   glUniform4i                 GLAPI_SET;
GLAPI_DECL PFNGLUNIFORM4FVPROC                  glUniform4fv                GLAPI_SET;
GLAPI_DECL PFNGLUSEPROGRAMPROC                  glUseProgram                GLAPI_SET;
GLAPI_DECL PFNGLVERTEXATTRIBIPOINTERPROC        glVertexAttribIPointer      GLAPI_SET;
GLAPI_DECL PFNGLVERTEXATTRIBPOINTERPROC         glVertexAttribPointer       GLAPI_SET;
GLAPI_DECL PFNGLDRAWARRAYSINSTANCEDPROC         glDrawArraysInstanced       GLAPI_SET;
GLAPI_DECL PFNGLVERTEXATTRIBDIVISORPROC         glVertexAttribDivisor       GLAPI_SET;
GLAPI_DECL PFNGLBLENDFUNCSEPARATEPROC           glBlendFuncSeparate         GLAPI_SET;

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
    void SetTexture(uint16 index, GLenum type, GLuint texture);
}

namespace OpenGLState
{
    extern uint16 ActiveTexture;
    extern GLuint CurrentProgram;

    void Reset();
}
