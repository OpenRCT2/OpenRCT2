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

#ifndef OPENGL_PROC
#error "Do not include OpenGLAPIProc.h directly. Include OpenGLAPI.h instead."
#endif

// 1.1 function pointers
OPENGL_PROC(PFNGLACTIVETEXTUREPROC,             glActiveTexture)
OPENGL_PROC(PFNGLBEGINPROC,                     glBegin)
OPENGL_PROC(PFNGLBINDTEXTUREPROC,               glBindTexture)
OPENGL_PROC(PFNGLBLENDFUNCPROC,                 glBlendFunc)
OPENGL_PROC(PFNGLCLEARPROC,                     glClear)
OPENGL_PROC(PFNGLCLEARCOLORPROC,                glClearColor)
OPENGL_PROC(PFNGLCULLFACEPROC,                  glCullFace)
OPENGL_PROC(PFNGLDELETETEXTURESPROC,            glDeleteTextures)
OPENGL_PROC(PFNGLDISABLEPROC,                   glDisable)
OPENGL_PROC(PFNGLDRAWARRAYSPROC,                glDrawArrays)
OPENGL_PROC(PFNGLENABLEPROC,                    glEnable)
OPENGL_PROC(PFNGLENDPROC,                       glEnd)
OPENGL_PROC(PFNGLGENTEXTURESPROC,               glGenTextures)
OPENGL_PROC(PFNGLGETERRORPROC,                  glGetError)
OPENGL_PROC(PFNGLPIXELSTOREIPROC,               glPixelStorei)
OPENGL_PROC(PFNGLREADPIXELSPROC,                glReadPixels)
OPENGL_PROC(PFNGLTEXIMAGE2DPROC,                glTexImage2D)
OPENGL_PROC(PFNGLTEXPARAMETERIPROC,             glTexParameteri)
OPENGL_PROC(PFNGLVIEWPORTPROC,                  glViewport)
OPENGL_PROC(PFNGLTEXSUBIMAGE3DPROC,             glTexSubImage3D)
OPENGL_PROC(PFNGLTEXIMAGE3DPROC,                glTexImage3D)
OPENGL_PROC(PFNGLGETINTERGERVPROC,              glGetIntegerv)
OPENGL_PROC(PFNGLGETTEXIMAGEPROC,               glGetTexImage)

// 2.0+ function pointers
OPENGL_PROC(PFNGLATTACHSHADERPROC,              glAttachShader)
OPENGL_PROC(PFNGLBINDBUFFERPROC,                glBindBuffer)
OPENGL_PROC(PFNGLBINDFRAGDATALOCATIONPROC,      glBindFragDataLocation)
OPENGL_PROC(PFNGLBINDFRAMEBUFFERPROC,           glBindFramebuffer)
OPENGL_PROC(PFNGLBINDVERTEXARRAYPROC,           glBindVertexArray)
OPENGL_PROC(PFNGLBUFFERDATAPROC,                glBufferData)
OPENGL_PROC(PFNGLCOMPILESHADERPROC,             glCompileShader)
OPENGL_PROC(PFNGLCREATEPROGRAMPROC,             glCreateProgram)
OPENGL_PROC(PFNGLCREATESHADERPROC,              glCreateShader)
OPENGL_PROC(PFNGLDELETEBUFFERSPROC,             glDeleteBuffers)
OPENGL_PROC(PFNGLDELETEFRAMEBUFFERSPROC,        glDeleteFramebuffers)
OPENGL_PROC(PFNGLDELETEPROGRAMPROC,             glDeleteProgram)
OPENGL_PROC(PFNGLDELETESHADERPROC,              glDeleteShader)
OPENGL_PROC(PFNGLDELETEVERTEXARRAYSPROC,        glDeleteVertexArrays)
OPENGL_PROC(PFNGLDETACHSHADERPROC,              glDetachShader)
OPENGL_PROC(PFNGLENABLEVERTEXATTRIBARRAYPROC,   glEnableVertexAttribArray)
OPENGL_PROC(PFNGLFRAMEBUFFERTEXTURE2DPROC,      glFramebufferTexture2D)
OPENGL_PROC(PFNGLGETATTRIBLOCATIONPROC,         glGetAttribLocation)
OPENGL_PROC(PFNGLGENBUFFERSPROC,                glGenBuffers)
OPENGL_PROC(PFNGLGENFRAMEBUFFERSPROC,           glGenFramebuffers)
OPENGL_PROC(PFNGLGETPROGRAMINFOLOGPROC,         glGetProgramInfoLog)
OPENGL_PROC(PFNGLGETPROGRAMIVPROC,              glGetProgramiv)
OPENGL_PROC(PFNGLGETSHADERINFOLOGPROC,          glGetShaderInfoLog)
OPENGL_PROC(PFNGLGETSHADERIVPROC,               glGetShaderiv)
OPENGL_PROC(PFNGLGETUNIFORMLOCATIONPROC,        glGetUniformLocation)
OPENGL_PROC(PFNGLGENVERTEXARRAYSPROC,           glGenVertexArrays)
OPENGL_PROC(PFNGLLINKPROGRAMPROC,               glLinkProgram)
OPENGL_PROC(PFNGLSHADERSOURCEPROC,              glShaderSource)
OPENGL_PROC(PFNGLUNIFORM1IPROC,                 glUniform1i)
OPENGL_PROC(PFNGLUNIFORM1IVPROC,                glUniform1iv)
OPENGL_PROC(PFNGLUNIFORM2IPROC,                 glUniform2i)
OPENGL_PROC(PFNGLUNIFORM2FPROC,                 glUniform2f)
OPENGL_PROC(PFNGLUNIFORM4FPROC,                 glUniform4f)
OPENGL_PROC(PFNGLUNIFORM4IPROC,                 glUniform4i)
OPENGL_PROC(PFNGLUNIFORM4FVPROC,                glUniform4fv)
OPENGL_PROC(PFNGLUSEPROGRAMPROC,                glUseProgram)
OPENGL_PROC(PFNGLVERTEXATTRIBIPOINTERPROC,      glVertexAttribIPointer)
OPENGL_PROC(PFNGLVERTEXATTRIBPOINTERPROC,       glVertexAttribPointer)
OPENGL_PROC(PFNGLDRAWARRAYSINSTANCEDPROC,       glDrawArraysInstanced)
OPENGL_PROC(PFNGLVERTEXATTRIBDIVISORPROC,       glVertexAttribDivisor)
OPENGL_PROC(PFNGLBLENDFUNCSEPARATEPROC,         glBlendFuncSeparate)
