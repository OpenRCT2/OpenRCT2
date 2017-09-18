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

#include "DrawImageShader.h"

DrawImageShader::DrawImageShader() : OpenGLShaderProgram("drawimage")
{
    GetLocations();

    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_vboInstances);
    glGenVertexArrays(1, &_vao);

    GLuint vertices[] = { 0, 1, 2, 2, 1, 3 };
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(_vao);

    glVertexAttribIPointer(vIndex, 1, GL_INT, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);
    glVertexAttribIPointer(vClip, 4, GL_INT, sizeof(DrawImageCommand), (void*) offsetof(DrawImageCommand, clip));
    glVertexAttribIPointer(vTexColourAtlas, 1, GL_INT, sizeof(DrawImageCommand), (void*) offsetof(DrawImageCommand, texColourAtlas));
    glVertexAttribPointer(vTexColourBounds, 4, GL_FLOAT, GL_FALSE, sizeof(DrawImageCommand), (void*) offsetof(DrawImageCommand, texColourBounds));
    glVertexAttribIPointer(vTexMaskAtlas, 1, GL_INT, sizeof(DrawImageCommand), (void*) offsetof(DrawImageCommand, texMaskAtlas));
    glVertexAttribPointer(vTexMaskBounds, 4, GL_FLOAT, GL_FALSE, sizeof(DrawImageCommand), (void*) offsetof(DrawImageCommand, texMaskBounds));
    glVertexAttribIPointer(vTexPaletteAtlas, 1, GL_INT, sizeof(DrawImageCommand), (void*) offsetof(DrawImageCommand, texPaletteAtlas));
    glVertexAttribPointer(vTexPaletteBounds, 4, GL_FLOAT, GL_FALSE, sizeof(DrawImageCommand), (void*) offsetof(DrawImageCommand, texPaletteBounds));
    glVertexAttribIPointer(vFlags, 1, GL_INT, sizeof(DrawImageCommand), (void*) offsetof(DrawImageCommand, flags));
    glVertexAttribPointer(vColour, 4, GL_FLOAT, GL_FALSE, sizeof(DrawImageCommand), (void*) offsetof(DrawImageCommand, colour));
    glVertexAttribIPointer(vBounds, 4, GL_INT, sizeof(DrawImageCommand), (void*) offsetof(DrawImageCommand, bounds));
    glVertexAttribIPointer(vMask, 1, GL_INT, sizeof(DrawImageCommand), (void*) offsetof(DrawImageCommand, mask));

    glEnableVertexAttribArray(vIndex);
    glEnableVertexAttribArray(vClip);
    glEnableVertexAttribArray(vTexColourAtlas);
    glEnableVertexAttribArray(vTexColourBounds);
    glEnableVertexAttribArray(vTexMaskAtlas);
    glEnableVertexAttribArray(vTexMaskBounds);
    glEnableVertexAttribArray(vTexPaletteAtlas);
    glEnableVertexAttribArray(vTexPaletteBounds);
    glEnableVertexAttribArray(vFlags);
    glEnableVertexAttribArray(vColour);
    glEnableVertexAttribArray(vBounds);
    glEnableVertexAttribArray(vMask);

    glVertexAttribDivisor(vClip, 1);
    glVertexAttribDivisor(vTexColourAtlas, 1);
    glVertexAttribDivisor(vTexColourBounds, 1);
    glVertexAttribDivisor(vTexMaskAtlas, 1);
    glVertexAttribDivisor(vTexMaskBounds, 1);
    glVertexAttribDivisor(vTexPaletteAtlas, 1);
    glVertexAttribDivisor(vTexPaletteBounds, 1);
    glVertexAttribDivisor(vFlags, 1);
    glVertexAttribDivisor(vColour, 1);
    glVertexAttribDivisor(vBounds, 1);
    glVertexAttribDivisor(vMask, 1);

    Use();
    glUniform1i(uTexture, 0);
}

DrawImageShader::~DrawImageShader()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_vboInstances);
    glDeleteVertexArrays(1, &_vao);
}

void DrawImageShader::GetLocations()
{
    uScreenSize         = GetUniformLocation("uScreenSize");
    uTexture            = GetUniformLocation("uTexture");
    uPalette            = GetUniformLocation("uPalette");

    vIndex              = GetAttributeLocation("vIndex");
    vClip               = GetAttributeLocation("ivClip");
    vTexColourAtlas     = GetAttributeLocation("ivTexColourAtlas");
    vTexColourBounds    = GetAttributeLocation("ivTexColourBounds");
    vTexMaskAtlas       = GetAttributeLocation("ivTexMaskAtlas");
    vTexMaskBounds      = GetAttributeLocation("ivTexMaskBounds");
    vTexPaletteAtlas    = GetAttributeLocation("ivTexPaletteAtlas");
    vTexPaletteBounds   = GetAttributeLocation("ivTexPaletteBounds");
    vFlags              = GetAttributeLocation("ivFlags");
    vColour             = GetAttributeLocation("ivColour");
    vBounds             = GetAttributeLocation("ivBounds");
    vMask               = GetAttributeLocation("ivMask");
}

void DrawImageShader::SetScreenSize(sint32 width, sint32 height)
{
    glUniform2i(uScreenSize, width, height);
}

void DrawImageShader::SetPalette(const vec4f *glPalette)
{
    glUniform4fv(uPalette, 256, (const GLfloat *) glPalette);
}

void DrawImageShader::DrawInstances(const ImageCommandBatch& instances)
{
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DrawImageCommand) * instances.size(), instances.data(), GL_STREAM_DRAW);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (GLsizei)instances.size());
}

#endif /* DISABLE_OPENGL */
