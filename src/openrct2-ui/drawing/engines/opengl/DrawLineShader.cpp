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

#include "DrawLineShader.h"
#include "OpenGLFramebuffer.h"

namespace
{
    struct VDStruct
    {
        GLfloat mat[4][2];
    };
}

constexpr VDStruct VertexData[2] =
{
    { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
};

DrawLineShader::DrawLineShader() : OpenGLShaderProgram("drawline")
{
    GetLocations();

    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_vboInstances);
    glGenVertexArrays(1, &_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData, GL_STATIC_DRAW);

    glBindVertexArray(_vao);
    glVertexAttribPointer(vVertMat+0, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), (void*) offsetof(VDStruct, mat[0]));
    glVertexAttribPointer(vVertMat+1, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), (void*) offsetof(VDStruct, mat[1]));
    glVertexAttribPointer(vVertMat+2, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), (void*) offsetof(VDStruct, mat[2]));
    glVertexAttribPointer(vVertMat+3, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), (void*) offsetof(VDStruct, mat[3]));

    glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);
    glVertexAttribIPointer(vClip, 4, GL_INT, sizeof(DrawLineCommand), (void*) offsetof(DrawLineCommand, clip));
    glVertexAttribIPointer(vBounds, 4, GL_INT, sizeof(DrawLineCommand), (void*) offsetof(DrawLineCommand, bounds));
    glVertexAttribIPointer(vColour, 1, GL_UNSIGNED_INT, sizeof(DrawLineCommand), (void*) offsetof(DrawLineCommand, colour));
    glVertexAttribIPointer(vDepth, 1, GL_INT, sizeof(DrawLineCommand), (void*) offsetof(DrawLineCommand, depth));

    glEnableVertexAttribArray(vVertMat+0);
    glEnableVertexAttribArray(vVertMat+1);
    glEnableVertexAttribArray(vVertMat+2);
    glEnableVertexAttribArray(vVertMat+3);

    glEnableVertexAttribArray(vClip);
    glEnableVertexAttribArray(vBounds);
    glEnableVertexAttribArray(vColour);
    glEnableVertexAttribArray(vDepth);

    glVertexAttribDivisor(vClip, 1);
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

    vClip       = GetAttributeLocation("vClip");
    vBounds     = GetAttributeLocation("vBounds");
    vColour     = GetAttributeLocation("vColour");
    vDepth      = GetAttributeLocation("vDepth");

    vVertMat    = GetAttributeLocation("vVertMat");
}

void DrawLineShader::SetScreenSize(sint32 width, sint32 height)
{
    glUniform2i(uScreenSize, width, height);
}

void DrawLineShader::DrawInstances(const LineCommandBatch& instances)
{
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DrawLineCommand) * instances.size(), instances.data(), GL_STREAM_DRAW);

    glDrawArraysInstanced(GL_LINES, 0, 2, (GLsizei)instances.size());
}

#endif /* DISABLE_OPENGL */
