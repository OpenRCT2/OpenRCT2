/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

    #include "DrawLineShader.h"

    #include "OpenGLFramebuffer.h"

using namespace OpenRCT2::Ui;

namespace
{
    struct VDStruct
    {
        GLfloat mat[4][2];
    };
} // namespace

constexpr VDStruct kVertexData[2] = {
    { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
};

DrawLineShader::DrawLineShader()
    : OpenGLShaderProgram("drawline")
{
    GetLocations();

    glCall(glGenBuffers, 1, &_vbo);
    glCall(glGenBuffers, 1, &_vboInstances);
    glCall(glGenVertexArrays, 1, &_vao);

    glCall(glBindBuffer, GL_ARRAY_BUFFER, _vbo);
    glCall(glBufferData, GL_ARRAY_BUFFER, sizeof(kVertexData), kVertexData, GL_STATIC_DRAW);
    glCall(glBindVertexArray, _vao);
    glCall(
        glVertexAttribPointer, vVertMat + 0, 2, GL_FLOAT, GL_FALSE, glSizeOf<VDStruct>(),
        reinterpret_cast<void*>(offsetof(VDStruct, mat[0])));
    glCall(
        glVertexAttribPointer, vVertMat + 1, 2, GL_FLOAT, GL_FALSE, glSizeOf<VDStruct>(),
        reinterpret_cast<void*>(offsetof(VDStruct, mat[1])));
    glCall(
        glVertexAttribPointer, vVertMat + 2, 2, GL_FLOAT, GL_FALSE, glSizeOf<VDStruct>(),
        reinterpret_cast<void*>(offsetof(VDStruct, mat[2])));
    glCall(
        glVertexAttribPointer, vVertMat + 3, 2, GL_FLOAT, GL_FALSE, glSizeOf<VDStruct>(),
        reinterpret_cast<void*>(offsetof(VDStruct, mat[3])));

    glCall(glBindBuffer, GL_ARRAY_BUFFER, _vboInstances);
    glCall(
        glVertexAttribIPointer, vBounds, 4, GL_INT, glSizeOf<DrawLineCommand>(),
        reinterpret_cast<void*>(offsetof(DrawLineCommand, bounds)));
    glCall(
        glVertexAttribIPointer, vColour, 1, GL_UNSIGNED_INT, glSizeOf<DrawLineCommand>(),
        reinterpret_cast<void*>(offsetof(DrawLineCommand, colour)));
    glCall(
        glVertexAttribIPointer, vDepth, 1, GL_INT, glSizeOf<DrawLineCommand>(),
        reinterpret_cast<void*>(offsetof(DrawLineCommand, depth)));

    glCall(glEnableVertexAttribArray, vVertMat + 0);
    glCall(glEnableVertexAttribArray, vVertMat + 1);
    glCall(glEnableVertexAttribArray, vVertMat + 2);
    glCall(glEnableVertexAttribArray, vVertMat + 3);

    glCall(glEnableVertexAttribArray, vBounds);
    glCall(glEnableVertexAttribArray, vColour);
    glCall(glEnableVertexAttribArray, vDepth);

    glCall(glVertexAttribDivisor, vBounds, 1);
    glCall(glVertexAttribDivisor, vColour, 1);
    glCall(glVertexAttribDivisor, vDepth, 1);

    Use();
}

DrawLineShader::~DrawLineShader()
{
    glCall(glDeleteBuffers, 1, &_vbo);
    glCall(glDeleteVertexArrays, 1, &_vao);
}

void DrawLineShader::GetLocations()
{
    uScreenSize = GetUniformLocation("uScreenSize");

    vBounds = GetAttributeLocation("vBounds");
    vColour = GetAttributeLocation("vColour");
    vDepth = GetAttributeLocation("vDepth");

    vVertMat = GetAttributeLocation("vVertMat");
}

void DrawLineShader::SetScreenSize(int32_t width, int32_t height)
{
    glCall(glUniform2i, uScreenSize, width, height);
}

void DrawLineShader::DrawInstances(const LineCommandBatch& instances)
{
    glCall(glBindVertexArray, _vao);

    glCall(glBindBuffer, GL_ARRAY_BUFFER, _vboInstances);
    glCall(glBufferData, GL_ARRAY_BUFFER, sizeof(DrawLineCommand) * instances.size(), instances.data(), GL_STREAM_DRAW);

    glCall(glDrawArraysInstanced, GL_LINES, 0, 2, static_cast<GLsizei>(instances.size()));
}

#endif /* DISABLE_OPENGL */
