/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "OpenGLAPI.h"

#include <chrono>
#include <filesystem>
#include <memory>
#include <openrct2/core/StringTypes.h>
#include <string>

namespace OpenRCT2::Ui
{
    class OpenGLShader final
    {
    private:
        GLenum _type;
        GLuint _id = 0;
        std::filesystem::file_time_type _lastWriteTime;
        std::filesystem::path _filePath;

    public:
        OpenGLShader(const char* name, GLenum type);
        ~OpenGLShader();

        GLuint GetShaderId();

        bool NeedsReload() const;
    };

    class OpenGLShaderProgram
    {
        using Clock = std::chrono::steady_clock;

    private:
        std::string _name;
        GLuint _id = 0;
        std::unique_ptr<OpenGLShader> _vertexShader;
        std::unique_ptr<OpenGLShader> _fragmentShader;
        Clock::time_point _lastReloadCheck{};

    public:
        explicit OpenGLShaderProgram(u8string_view name);
        explicit OpenGLShaderProgram(const OpenGLShaderProgram&) = delete;
        explicit OpenGLShaderProgram(OpenGLShaderProgram&&) = default;
        virtual ~OpenGLShaderProgram();

        GLuint GetAttributeLocation(const char* name);
        GLuint GetUniformLocation(const char* name);
        void Use();
        void Reload();
        void Destroy();
        void Update();

    private:
        bool Link();
    };
} // namespace OpenRCT2::Ui
