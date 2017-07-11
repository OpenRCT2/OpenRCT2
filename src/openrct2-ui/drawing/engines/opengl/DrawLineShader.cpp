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

DrawLineShader::DrawLineShader() : OpenGLShaderProgram("drawline")
{
    GetLocations();

    glGenBuffers(1, &_vbo);
    glGenVertexArrays(1, &_vao);

    GLuint vertices[] = { 0, 1 };
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(_vao);
    glEnableVertexAttribArray(vIndex);
    glVertexAttribIPointer(vIndex, 1, GL_INT, 0, nullptr);

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
    uClip       = GetUniformLocation("uClip");
    uBounds     = GetUniformLocation("uBounds");
    uColour     = GetUniformLocation("uColour");

    vIndex      = GetAttributeLocation("vIndex");
}

void DrawLineShader::SetScreenSize(sint32 width, sint32 height)
{
    glUniform2i(uScreenSize, width, height);
}

void DrawLineShader::SetClip(sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    glUniform4i(uClip, left, top, right, bottom);
}

void DrawLineShader::SetBounds(sint32 x0, sint32 y0, sint32 x1, sint32 y1)
{
    glUniform4i(uBounds, x0, y0, x1, y1);
}

void DrawLineShader::SetColour(vec4f colour)
{
    glUniform4f(uColour, colour.r, colour.g, colour.b, colour.a);
}

void DrawLineShader::Draw(sint32 x0, sint32 y0, sint32 x1, sint32 y1)
{
    SetBounds(x0, y0, x1, y1);

    glBindVertexArray(_vao);
    glDrawArrays(GL_LINES, 0, 2);
}

#endif /* DISABLE_OPENGL */
