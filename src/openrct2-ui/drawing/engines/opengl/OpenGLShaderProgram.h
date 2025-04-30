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

#include <memory>
#include <string>
namespace OpenRCT2::Ui
{
    class OpenGLShader final
    {
    private:
        static constexpr uint64_t kMaxSourceSize = 8 * 1024 * 1024; // 8 MiB

        GLenum _type;
        GLuint _id = 0;

    public:
        OpenGLShader(const char* name, GLenum type);
        ~OpenGLShader();

        GLuint GetShaderId();

    private:
        std::string GetPath(const std::string& name);
        static std::string ReadSourceCode(const std::string& path);
    };

    class OpenGLShaderProgram
    {
    private:
        GLuint _id = 0;
        std::unique_ptr<OpenGLShader> _vertexShader;
        std::unique_ptr<OpenGLShader> _fragmentShader;

    public:
        explicit OpenGLShaderProgram(const char* name);
        explicit OpenGLShaderProgram(const OpenGLShaderProgram&) = delete;
        explicit OpenGLShaderProgram(OpenGLShaderProgram&&) = default;
        virtual ~OpenGLShaderProgram();

        GLuint GetAttributeLocation(const char* name);
        GLuint GetUniformLocation(const char* name);
        void Use();

    private:
        bool Link();
    };
} // namespace OpenRCT2::Ui
