/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

    #include "DrawRectShader.h"

using namespace OpenRCT2::Ui;

namespace
{
    struct VDStruct
    {
        GLfloat mat[4][2];
        GLfloat vec[2];
    };
} // namespace

constexpr VDStruct kVertexData[4] = {
    { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
};

constexpr size_t kInitialInstancesBufferSize = 32768;

DrawRectShader::DrawRectShader()
    : OpenGLShaderProgram("drawrect")
    , _maxInstancesBufferSize(kInitialInstancesBufferSize)
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
    glVertexAttribPointer(vVertVec, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), reinterpret_cast<void*>(offsetof(VDStruct, vec)));

    glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DrawRectCommand) * kInitialInstancesBufferSize, nullptr, GL_STREAM_DRAW);

    glVertexAttribIPointer(vClip, 4, GL_INT, sizeof(DrawRectCommand), reinterpret_cast<void*>(offsetof(DrawRectCommand, clip)));
    glVertexAttribIPointer(
        vTexColourAtlas, 1, GL_INT, sizeof(DrawRectCommand),
        reinterpret_cast<void*>(offsetof(DrawRectCommand, texColourAtlas)));
    glVertexAttribPointer(
        vTexColourBounds, 4, GL_FLOAT, GL_FALSE, sizeof(DrawRectCommand),
        reinterpret_cast<void*>(offsetof(DrawRectCommand, texColourBounds)));
    glVertexAttribIPointer(
        vTexMaskAtlas, 1, GL_INT, sizeof(DrawRectCommand), reinterpret_cast<void*>(offsetof(DrawRectCommand, texMaskAtlas)));
    glVertexAttribPointer(
        vTexMaskBounds, 4, GL_FLOAT, GL_FALSE, sizeof(DrawRectCommand),
        reinterpret_cast<void*>(offsetof(DrawRectCommand, texMaskBounds)));
    glVertexAttribIPointer(
        vPalettes, 3, GL_INT, sizeof(DrawRectCommand), reinterpret_cast<void*>(offsetof(DrawRectCommand, palettes)));
    glVertexAttribIPointer(
        vFlags, 1, GL_INT, sizeof(DrawRectCommand), reinterpret_cast<void*>(offsetof(DrawRectCommand, flags)));
    glVertexAttribIPointer(
        vColour, 1, GL_UNSIGNED_INT, sizeof(DrawRectCommand), reinterpret_cast<void*>(offsetof(DrawRectCommand, colour)));
    glVertexAttribIPointer(
        vBounds, 4, GL_INT, sizeof(DrawRectCommand), reinterpret_cast<void*>(offsetof(DrawRectCommand, bounds)));
    glVertexAttribIPointer(
        vDepth, 1, GL_INT, sizeof(DrawRectCommand), reinterpret_cast<void*>(offsetof(DrawRectCommand, depth)));

    glEnableVertexAttribArray(vVertMat + 0);
    glEnableVertexAttribArray(vVertMat + 1);
    glEnableVertexAttribArray(vVertMat + 2);
    glEnableVertexAttribArray(vVertMat + 3);
    glEnableVertexAttribArray(vVertVec);

    glEnableVertexAttribArray(vClip);
    glEnableVertexAttribArray(vTexColourAtlas);
    glEnableVertexAttribArray(vTexColourBounds);
    glEnableVertexAttribArray(vTexMaskAtlas);
    glEnableVertexAttribArray(vTexMaskBounds);
    glEnableVertexAttribArray(vPalettes);
    glEnableVertexAttribArray(vFlags);
    glEnableVertexAttribArray(vColour);
    glEnableVertexAttribArray(vBounds);
    glEnableVertexAttribArray(vDepth);

    glVertexAttribDivisor(vClip, 1);
    glVertexAttribDivisor(vTexColourAtlas, 1);
    glVertexAttribDivisor(vTexColourBounds, 1);
    glVertexAttribDivisor(vTexMaskAtlas, 1);
    glVertexAttribDivisor(vTexMaskBounds, 1);
    glVertexAttribDivisor(vPalettes, 1);
    glVertexAttribDivisor(vFlags, 1);
    glVertexAttribDivisor(vColour, 1);
    glVertexAttribDivisor(vBounds, 1);
    glVertexAttribDivisor(vDepth, 1);

    Use();
    glUniform1i(uTexture, 0);
    glUniform1i(uPaletteTex, 1);

    glUniform1i(uPeelingTex, 2);
    glUniform1i(uPeeling, 0);
}

DrawRectShader::~DrawRectShader()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_vboInstances);
    glDeleteVertexArrays(1, &_vao);
}

void DrawRectShader::GetLocations()
{
    uScreenSize = GetUniformLocation("uScreenSize");
    uTexture = GetUniformLocation("uTexture");
    uPaletteTex = GetUniformLocation("uPaletteTex");

    uPeelingTex = GetUniformLocation("uPeelingTex");
    uPeeling = GetUniformLocation("uPeeling");

    vClip = GetAttributeLocation("vClip");
    vTexColourAtlas = GetAttributeLocation("vTexColourAtlas");
    vTexColourBounds = GetAttributeLocation("vTexColourBounds");
    vTexMaskAtlas = GetAttributeLocation("vTexMaskAtlas");
    vTexMaskBounds = GetAttributeLocation("vTexMaskBounds");
    vPalettes = GetAttributeLocation("vPalettes");
    vFlags = GetAttributeLocation("vFlags");
    vColour = GetAttributeLocation("vColour");
    vBounds = GetAttributeLocation("vBounds");
    vDepth = GetAttributeLocation("vDepth");

    vVertMat = GetAttributeLocation("vVertMat");
    vVertVec = GetAttributeLocation("vVertVec");
}

void DrawRectShader::SetScreenSize(int32_t width, int32_t height)
{
    glUniform2i(uScreenSize, width, height);
}

void DrawRectShader::EnablePeeling(GLuint peelingTex)
{
    OpenGLAPI::SetTexture(2, GL_TEXTURE_2D, peelingTex);
    glUniform1i(uPeeling, 1);
}

void DrawRectShader::DisablePeeling()
{
    glUniform1i(uPeeling, 0);
}

void DrawRectShader::SetInstances(const RectCommandBatch& instances)
{
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);

    if (instances.size() > _maxInstancesBufferSize)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(DrawRectCommand) * instances.size(), instances.data(), GL_STREAM_DRAW);
        _maxInstancesBufferSize = instances.size();
    }
    else
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DrawRectCommand) * instances.size(), instances.data());
    }

    _instanceCount = static_cast<GLsizei>(instances.size());
}

void DrawRectShader::DrawInstances()
{
    glBindVertexArray(_vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, _instanceCount);
}

#endif /* DISABLE_OPENGL */
