#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
    glEnableVertexAttribArray(vIndex);
    glVertexAttribIPointer(vIndex, 1, GL_INT, 0, nullptr);

    Use();
    SetTextureCoordinates(0, 0, 1, 1);
    glUniform1i(uTexture, 0);
}

DrawImageShader::~DrawImageShader()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_vboInstances);
    glDeleteVertexArrays(1, &_vao);

    glBindVertexArray(_vao);
}

void DrawImageShader::GetLocations()
{
    uScreenSize         = GetUniformLocation("uScreenSize");
    uTexture            = GetUniformLocation("uTexture");
    uPalette            = GetUniformLocation("uPalette");
    uTextureCoordinates = GetUniformLocation("uTextureCoordinates");

    vIndex              = GetAttributeLocation("vIndex");
}

void DrawImageShader::SetScreenSize(sint32 width, sint32 height)
{
    glUniform2i(uScreenSize, width, height);
}

void DrawImageShader::SetPalette(const vec4f *glPalette)
{
    glUniform4fv(uPalette, 256, (const GLfloat *) glPalette);
}

void DrawImageShader::SetTextureCoordinates(sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    glUniform4i(uTextureCoordinates, left, top, right, bottom);
}

void DrawImageShader::DrawInstances(const std::vector<DrawImageInstance>& instances)
{
    // Copy instance data to vbo for single-use
    glBindBuffer(GL_ARRAY_BUFFER, _vboInstances);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instances[0]) * instances.size(), instances.data(), GL_STREAM_DRAW);

    // Bind vertex attributes
    glBindVertexArray(_vao);

    glVertexAttribIPointer(GetAttributeLocation("ivClip"), 4, GL_INT, sizeof(DrawImageInstance), (void*) offsetof(DrawImageInstance, clip));
    glVertexAttribPointer(GetAttributeLocation("ivTexCoordScale"), 2, GL_FLOAT, GL_FALSE, sizeof(DrawImageInstance), (void*) offsetof(DrawImageInstance, texCoordScale));
    glVertexAttribIPointer(GetAttributeLocation("ivTexSlot"), 1, GL_INT, sizeof(DrawImageInstance), (void*) offsetof(DrawImageInstance, texSlot));
    glVertexAttribIPointer(GetAttributeLocation("ivFlags"), 1, GL_INT, sizeof(DrawImageInstance), (void*) offsetof(DrawImageInstance, flags));
    glVertexAttribPointer(GetAttributeLocation("ivColour"), 4, GL_FLOAT, GL_FALSE, sizeof(DrawImageInstance), (void*) offsetof(DrawImageInstance, colour));
    glVertexAttribIPointer(GetAttributeLocation("ivBounds"), 4, GL_INT, sizeof(DrawImageInstance), (void*) offsetof(DrawImageInstance, bounds));

    glEnableVertexAttribArray(GetAttributeLocation("ivClip"));
    glEnableVertexAttribArray(GetAttributeLocation("ivTexCoordScale"));
    glEnableVertexAttribArray(GetAttributeLocation("ivTexSlot"));
    glEnableVertexAttribArray(GetAttributeLocation("ivFlags"));
    glEnableVertexAttribArray(GetAttributeLocation("ivColour"));
    glEnableVertexAttribArray(GetAttributeLocation("ivBounds"));

    glVertexAttribDivisor(GetAttributeLocation("ivClip"), 1);
    glVertexAttribDivisor(GetAttributeLocation("ivTexCoordScale"), 1);
    glVertexAttribDivisor(GetAttributeLocation("ivTexSlot"), 1);
    glVertexAttribDivisor(GetAttributeLocation("ivFlags"), 1);
    glVertexAttribDivisor(GetAttributeLocation("ivColour"), 1);
    glVertexAttribDivisor(GetAttributeLocation("ivBounds"), 1);

    // Draw instances
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instances.size());
}

#endif /* DISABLE_OPENGL */
