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

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

static constexpr uint64_t kMaxSourceSize = 8 * 1024 * 1024; // 8 MiB

static std::filesystem::path GetPath(u8string_view name, GLenum type)
{
    auto env = GetContext()->GetPlatformEnvironment();
    auto shadersPath = env->GetDirectoryPath(DIRBASE::OPENRCT2, DIRID::SHADER);
    auto path = Path::Combine(shadersPath, name);
    if (type == GL_VERTEX_SHADER)
    {
        path += ".vert";
    }
    else
    {
        path += ".frag";
    }
    return std::filesystem::path{ path };
}

static std::string ReadSourceCode(const std::filesystem::path& path)
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

OpenGLShader::OpenGLShader(const char* name, GLenum type)
    : _type(type)
{
    _filePath = GetPath(name, type);
    _lastWriteTime = std::filesystem::last_write_time(_filePath);

    const auto sourceCode = ReadSourceCode(_filePath);
    const auto* sourceCodeStr = sourceCode.c_str();

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

        Console::Error::WriteLine("Error compiling %s", _filePath.u8string().c_str());
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

bool OpenGLShader::NeedsReload() const
{
    return std::filesystem::last_write_time(_filePath) > _lastWriteTime;
}

OpenGLShaderProgram::OpenGLShaderProgram(u8string_view name)
    : _name{ name }
{
    Reload();
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
    Destroy();
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

void OpenGLShaderProgram::Reload()
{
    Destroy();

    _fragmentShader = std::make_unique<OpenGLShader>(_name.c_str(), GL_FRAGMENT_SHADER);

    std::error_code ec{};
    if (!std::filesystem::exists(GetPath(_name, GL_VERTEX_SHADER), ec))
    {
        // Use the default vertex shader for post processing.
        _vertexShader = std::make_unique<OpenGLShader>("pp", GL_VERTEX_SHADER);
    }
    else
    {
        _vertexShader = std::make_unique<OpenGLShader>(_name.c_str(), GL_VERTEX_SHADER);
    }

    _id = glCall(glCreateProgram);
    glCall(glAttachShader, _id, _vertexShader->GetShaderId());
    glCall(glAttachShader, _id, _fragmentShader->GetShaderId());
    glCall(glBindFragDataLocation, _id, 0, "oColour");

    if (glObjectLabel != nullptr)
    {
        glObjectLabel(GL_PROGRAM, _id, -1, _name.c_str());
        // Might fail, consume errors.
        while (glGetError() != GL_NO_ERROR)
            ;
    }

    if (!Link())
    {
        char buffer[512];
        GLsizei length;
        glCall(glGetProgramInfoLog, _id, GLsizei(sizeof(buffer)), &length, buffer);

        Console::Error::WriteLine("Error linking %s", _name.c_str());
        Console::Error::WriteLine(buffer);

        throw std::runtime_error("Failed to link OpenGL shader.");
    }

    _lastReloadCheck = Clock::now();
}

void OpenGLShaderProgram::Destroy()
{
    if (_vertexShader != nullptr)
    {
        glDetachShader(_id, _vertexShader->GetShaderId());
    }
    if (_fragmentShader != nullptr)
    {
        glDetachShader(_id, _fragmentShader->GetShaderId());
    }
    if (_id != 0)
    {
        glDeleteProgram(_id);
    }
}

void OpenGLShaderProgram::Update()
{
    auto reloadCheckDelta = Clock::now() - _lastReloadCheck;
    if (reloadCheckDelta < std::chrono::milliseconds(200))
    {
        return;
    }
    _lastReloadCheck = Clock::now();

    if (_vertexShader->NeedsReload() || _fragmentShader->NeedsReload())
    {
        Reload();
    }
}

#endif /* DISABLE_OPENGL */
