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

#pragma once

#include <openrct2/common.h>
#include "OpenGLAPI.h"

class OpenGLShader final
{
private:
    static constexpr uint64 MaxSourceSize = 8 * 1024 * 1024; // 8 MiB

    GLenum _type;
    GLuint _id      = 0;

public:
    OpenGLShader(const char * name, GLenum type);
    ~OpenGLShader();

    GLuint GetShaderId();

private:
    void GetPath(char * buffer, size_t bufferSize, const char * name);
    static char * ReadSourceCode(const utf8 * path);
};

class OpenGLShaderProgram
{
private:
    GLuint         _id              = 0;
    OpenGLShader * _vertexShader    = nullptr;
    OpenGLShader * _fragmentShader  = nullptr;

public:
    explicit OpenGLShaderProgram(const char * name);
    explicit OpenGLShaderProgram(const OpenGLShaderProgram&) = default;
    virtual ~OpenGLShaderProgram();

    GLuint  GetAttributeLocation(const char * name);
    GLuint  GetUniformLocation(const char * name);
    void    Use();

private:
    bool Link();
};
