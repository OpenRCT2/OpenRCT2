/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_vboInstances);
    glGenVertexArrays(1, &_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kVertexData), kVertexData, GL_STATIC_DRAW);

    glBindVertexArray(_vao);
    glVertexAttribPointer(
        vVertMat + 0, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), reinterpret_cast<void*>(offsetof(VDStruct, mat[0])));
    glVertexAttribPointer(
        vVertMat + 1, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), reinterpret_cast<void*>(offsetof(VDStruct, mat[1])));
    glVertexAttribPointer(
        vVertMat + 2, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), reinterpret_cast<void*>(offsetof(VDStruct, mat[2])));
    glVertexAttribPointer(
        vVertMat + 3, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), reinterpret_cast<void*>(offsetof(VDStruct, mat[3])));

    glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);
    glVertexAttribIPointer(
        vBounds, 4, GL_INT, sizeof(DrawLineCommand), reinterpret_cast<void*>(offsetof(DrawLineCommand, bounds)));
    glVertexAttribIPointer(
        vColour, 1, GL_UNSIGNED_INT, sizeof(DrawLineCommand), reinterpret_cast<void*>(offsetof(DrawLineCommand, colour)));
    glVertexAttribIPointer(
        vDepth, 1, GL_INT, sizeof(DrawLineCommand), reinterpret_cast<void*>(offsetof(DrawLineCommand, depth)));

    glEnableVertexAttribArray(vVertMat + 0);
    glEnableVertexAttribArray(vVertMat + 1);
    glEnableVertexAttribArray(vVertMat + 2);
    glEnableVertexAttribArray(vVertMat + 3);

    glEnableVertexAttribArray(vClip);
    glEnableVertexAttribArray(vBounds);
    glEnableVertexAttribArray(vColour);
    glEnableVertexAttribArray(vDepth);

    glVertexAttribDivisor(vBounds, 1);
    glVertexAttribDivisor(vColour, 1);
    glVertexAttribDivisor(vDepth, 1);

    Use();
}

DrawLineShader::~DrawLineShader()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
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
    glUniform2i(uScreenSize, width, height);
}

void DrawLineShader::DrawInstances(const LineCommandBatch& instances)
{
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DrawLineCommand) * instances.size(), instances.data(), GL_STREAM_DRAW);

    glDrawArraysInstanced(GL_LINES, 0, 2, static_cast<GLsizei>(instances.size()));
}

#endif /* DISABLE_OPENGL */
