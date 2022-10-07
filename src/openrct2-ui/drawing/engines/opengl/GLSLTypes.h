/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "OpenGLAPI.h"

#include <openrct2/common.h>

#pragma pack(push, 1)

namespace detail
{
    template<typename T_> struct Vec2
    {
        using ValueType = T_;

        union
        {
            ValueType x;
            ValueType s;
            ValueType r;
        };
        union
        {
            ValueType y;
            ValueType t;
            ValueType g;
        };
    };

    template struct Vec2<GLfloat>;
    template struct Vec2<GLint>;

    template<typename T_> struct Vec3
    {
        using ValueType = T_;

        union
        {
            ValueType x;
            ValueType s;
            ValueType r;
        };
        union
        {
            ValueType y;
            ValueType t;
            ValueType g;
        };
        union
        {
            ValueType z;
            ValueType p;
            ValueType b;
        };
    };

    template struct Vec3<GLfloat>;
    template struct Vec3<GLint>;

    template<typename T_> struct Vec4
    {
        using ValueType = T_;

        union
        {
            ValueType x;
            ValueType s;
            ValueType r;
        };
        union
        {
            ValueType y;
            ValueType t;
            ValueType g;
        };
        union
        {
            ValueType z;
            ValueType p;
            ValueType b;
        };
        union
        {
            ValueType w;
            ValueType q;
            ValueType a;
        };
    };

    template struct Vec4<GLfloat>;
    template struct Vec4<GLint>;

} // namespace detail

using vec2 = detail::Vec2<GLfloat>;
using ivec2 = detail::Vec2<GLint>;

using vec3 = detail::Vec3<GLfloat>;
using ivec3 = detail::Vec3<GLint>;

using vec4 = detail::Vec4<GLfloat>;
using ivec4 = detail::Vec4<GLint>;

#pragma pack(pop)
