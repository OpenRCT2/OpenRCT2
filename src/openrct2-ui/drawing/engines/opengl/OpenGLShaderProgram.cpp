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

#include <openrct2/core/Console.hpp>
#include <openrct2/core/Exception.hpp>
#include <openrct2/core/FileStream.hpp>
#include <openrct2/core/Memory.hpp>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include "OpenGLShaderProgram.h"

#include <openrct2/platform/platform.h>

OpenGLShader::OpenGLShader(const char * name, GLenum type)
{
    _type = type;

    utf8 path[MAX_PATH];
    GetPath(path, sizeof(path), name);
    char * sourceCode = ReadSourceCode(path);

    _id = glCreateShader(type);
    glShaderSource(_id, 1, (const GLchar**)&sourceCode, nullptr);
    glCompileShader(_id);

    Memory::Free(sourceCode);

    GLint status;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        char buffer[512];
        glGetShaderInfoLog(_id, sizeof(buffer), nullptr, buffer);
        glDeleteShader(_id);

        Console::Error::WriteLine("Error compiling %s", path);
        Console::Error::WriteLine(buffer);

        throw Exception("Error compiling shader.");
    }
}

OpenGLShader::~OpenGLShader()
{
    glDeleteShader(_id);
}

GLuint OpenGLShader::GetShaderId()
{
    return _id;
}

void OpenGLShader::GetPath(char * buffer, size_t bufferSize, const char * name)
{
    platform_get_openrct_data_path(buffer, bufferSize);
    Path::Append(buffer, bufferSize, "shaders");
    Path::Append(buffer, bufferSize, name);
    if (_type == GL_VERTEX_SHADER)
    {
        String::Append(buffer, bufferSize, ".vert");
    }
    else
    {
        String::Append(buffer, bufferSize, ".frag");
    }
}

char * OpenGLShader::ReadSourceCode(const utf8 * path)
{
    auto fs = FileStream(path, FILE_MODE_OPEN);

    uint64 fileLength = fs.GetLength();
    if (fileLength > MaxSourceSize)
    {
        throw IOException("Shader source too large.");
    }

    utf8 * fileData = Memory::Allocate<utf8>((size_t)fileLength + 1);
    fs.Read(fileData, fileLength);
    fileData[fileLength] = '\0';
    return fileData;
}

OpenGLShaderProgram::OpenGLShaderProgram(const char * name)
{
    _vertexShader = new OpenGLShader(name, GL_VERTEX_SHADER);
    _fragmentShader = new OpenGLShader(name, GL_FRAGMENT_SHADER);

    _id = glCreateProgram();
    glAttachShader(_id, _vertexShader->GetShaderId());
    glAttachShader(_id, _fragmentShader->GetShaderId());
    glBindFragDataLocation(_id, 0, "oColour");

    if (!Link())
    {
        char buffer[512];
        GLsizei length;
        glGetProgramInfoLog(_id, sizeof(buffer), &length, buffer);

        Console::Error::WriteLine("Error linking %s", name);
        Console::Error::WriteLine(buffer);

        throw Exception("Failed to link OpenGL shader.");
    }
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
    if (_vertexShader != nullptr)
    {
        glDetachShader(_id, _vertexShader->GetShaderId());
        delete _vertexShader;
    }
    if (_fragmentShader != nullptr)
    {
        glDetachShader(_id, _fragmentShader->GetShaderId());
        delete _fragmentShader;
    }
    glDeleteProgram(_id);
}

GLuint OpenGLShaderProgram::GetAttributeLocation(const char * name)
{
    return glGetAttribLocation(_id, name);
}

GLuint OpenGLShaderProgram::GetUniformLocation(const char * name)
{
    return glGetUniformLocation(_id, name);
}

void OpenGLShaderProgram::Use()
{
    if (OpenGLState::CurrentProgram != _id)
    {
        OpenGLState::CurrentProgram = _id;
        glUseProgram(_id);
    }
}

bool OpenGLShaderProgram::Link()
{
    glLinkProgram(_id);

    GLint linkStatus;
    glGetProgramiv(_id, GL_LINK_STATUS, &linkStatus);
    return linkStatus == GL_TRUE;
}

#endif /* DISABLE_OPENGL */
