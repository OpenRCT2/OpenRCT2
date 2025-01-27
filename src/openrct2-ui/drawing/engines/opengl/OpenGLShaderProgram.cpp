/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

    #include "OpenGLShaderProgram.h"

    #include <openrct2/Context.h>
    #include <openrct2/PlatformEnvironment.h>
    #include <openrct2/core/Console.hpp>
    #include <openrct2/core/FileStream.h>
    #include <openrct2/core/Path.hpp>
    #include <openrct2/core/String.hpp>

using namespace OpenRCT2::Ui;

OpenGLShader::OpenGLShader(const char* name, GLenum type)
    : _type(type)
{
    auto path = GetPath(name);
    auto sourceCode = ReadSourceCode(path);
    auto sourceCodeStr = sourceCode.c_str();

    _id = glCreateShader(type);
    glCall(glShaderSource, _id, 1, static_cast<const GLchar**>(&sourceCodeStr), nullptr);
    glCall(glCompileShader, _id);

    GLint status;
    glCall(glGetShaderiv, _id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        char buffer[512];
        glCall(glGetShaderInfoLog, _id, GLsizei(sizeof(buffer)), nullptr, buffer);
        glCall(glDeleteShader, _id);

        Console::Error::WriteLine("Error compiling %s", path.c_str());
        Console::Error::WriteLine(buffer);

        throw std::runtime_error("Error compiling shader.");
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

std::string OpenGLShader::GetPath(const std::string& name)
{
    auto env = GetContext()->GetPlatformEnvironment();
    auto shadersPath = env->GetDirectoryPath(DIRBASE::OPENRCT2, DIRID::SHADER);
    auto path = Path::Combine(shadersPath, name);
    if (_type == GL_VERTEX_SHADER)
    {
        path += ".vert";
    }
    else
    {
        path += ".frag";
    }
    return path;
}

std::string OpenGLShader::ReadSourceCode(const std::string& path)
{
    auto fs = FileStream(path, FILE_MODE_OPEN);

    uint64_t fileLength = fs.GetLength();
    if (fileLength > kMaxSourceSize)
    {
        throw IOException("Shader source too large.");
    }

    auto fileData = std::string(static_cast<size_t>(fileLength) + 1, '\0');
    fs.Read(static_cast<void*>(fileData.data()), fileLength);
    return fileData;
}

OpenGLShaderProgram::OpenGLShaderProgram(const char* name)
{
    _vertexShader = std::make_unique<OpenGLShader>(name, GL_VERTEX_SHADER);
    _fragmentShader = std::make_unique<OpenGLShader>(name, GL_FRAGMENT_SHADER);

    _id = glCall(glCreateProgram);
    glCall(glAttachShader, _id, _vertexShader->GetShaderId());
    glCall(glAttachShader, _id, _fragmentShader->GetShaderId());
    glCall(glBindFragDataLocation, _id, 0, "oColour");

    if (glObjectLabel != nullptr)
    {
        glObjectLabel(GL_PROGRAM, _id, -1, name);
    }

    if (!Link())
    {
        char buffer[512];
        GLsizei length;
        glCall(glGetProgramInfoLog, _id, GLsizei(sizeof(buffer)), &length, buffer);

        Console::Error::WriteLine("Error linking %s", name);
        Console::Error::WriteLine(buffer);

        throw std::runtime_error("Failed to link OpenGL shader.");
    }
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
    if (_vertexShader != nullptr)
    {
        glDetachShader(_id, _vertexShader->GetShaderId());
    }
    if (_fragmentShader != nullptr)
    {
        glDetachShader(_id, _fragmentShader->GetShaderId());
    }
    glDeleteProgram(_id);
}

GLuint OpenGLShaderProgram::GetAttributeLocation(const char* name)
{
    return glCall(glGetAttribLocation, _id, name);
}

GLuint OpenGLShaderProgram::GetUniformLocation(const char* name)
{
    return glCall(glGetUniformLocation, _id, name);
}

void OpenGLShaderProgram::Use()
{
    if (OpenGLState::CurrentProgram != _id)
    {
        OpenGLState::CurrentProgram = _id;
        glCall(glUseProgram, _id);
    }
}

bool OpenGLShaderProgram::Link()
{
    glCall(glLinkProgram, _id);

    GLint linkStatus;
    glCall(glGetProgramiv, _id, GL_LINK_STATUS, &linkStatus);
    return linkStatus == GL_TRUE;
}

#endif /* DISABLE_OPENGL */
