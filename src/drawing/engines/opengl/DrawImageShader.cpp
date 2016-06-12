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
    glGenVertexArrays(1, &_vao);

    GLuint vertices[] = { 0, 1, 2, 2, 1, 3 };
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(_vao);
    glEnableVertexAttribArray(vIndex);
    glVertexAttribIPointer(vIndex, 1, GL_INT, 0, nullptr);

    Use();
    SetFlags(0);
    SetTextureCoordinates(0, 0, 1, 1);
    glUniform1i(uTexture, 0);
}

DrawImageShader::~DrawImageShader()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);

    glBindVertexArray(_vao);
}

void DrawImageShader::GetLocations()
{
    uScreenSize         = GetUniformLocation("uScreenSize");
    uClip               = GetUniformLocation("uClip");
    uBounds             = GetUniformLocation("uBounds");
    uTextureCoordinates = GetUniformLocation("uTextureCoordinates");
    uTexture            = GetUniformLocation("uTexture");
    uColour             = GetUniformLocation("uColour");
    uFlags              = GetUniformLocation("uFlags");
    uPalette            = GetUniformLocation("uPalette");

    vIndex              = GetAttributeLocation("vIndex");
}

void DrawImageShader::SetScreenSize(sint32 width, sint32 height)
{
    glUniform2i(uScreenSize, width, height);
}

void DrawImageShader::SetClip(sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    glUniform4i(uClip, left, top, right, bottom);
}

void DrawImageShader::SetBounds(sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    glUniform4i(uBounds, left, top, right, bottom);
}

void DrawImageShader::SetTextureCoordinates(sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    glUniform4i(uTextureCoordinates, left, top, right, bottom);
}

void DrawImageShader::SetTexture(GLuint texture)
{
    OpenGLAPI::SetTexture2D(0, texture);
}

void DrawImageShader::SetColour(vec4f colour)
{
    glUniform4f(uColour, colour.r, colour.g, colour.b, colour.a);
}

void DrawImageShader::SetFlags(uint32 flags)
{
    glUniform1i(uFlags, flags);
}

void DrawImageShader::SetPalette(const vec4f *glPalette)
{
    glUniform4fv(uPalette, 256, (const GLfloat *) glPalette);
}

void DrawImageShader::Draw(sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    SetBounds(left, top, right, bottom);

    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

#endif /* DISABLE_OPENGL */
