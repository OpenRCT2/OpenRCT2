/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GLSLTypes.h"
#include "OpenGLAPI.h"
#include "TextureCache.h"

#include <vector>
namespace OpenRCT2::Ui
{
    template<typename T>
    class CommandBatch
    {
    private:
        std::vector<T> _instances;
        size_t _numInstances;

    public:
        CommandBatch()
            : _numInstances(0)
        {
        }

        [[nodiscard]] bool empty() const // NOLINT(readability-identifier-naming)
        {
            return _numInstances == 0;
        }
        void clear() // NOLINT(readability-identifier-naming)
        {
            _numInstances = 0;
        }
        T& allocate() // NOLINT(readability-identifier-naming)
        {
            if (_numInstances + 1 > _instances.size())
            {
                _instances.resize((_numInstances + 1) << 1);
            }
            return _instances[_numInstances++];
        }
        T& insert(const T& value) // NOLINT(readability-identifier-naming)
        {
            if (_numInstances + 1 > _instances.size())
            {
                _instances.resize((_numInstances + 1) << 1);
            }
            return _instances[_numInstances++] = value;
        }
        [[nodiscard]] size_t size() const // NOLINT(readability-identifier-naming)
        {
            return _numInstances;
        }
        const T* data() const // NOLINT(readability-identifier-naming)
        {
            return _instances.data();
        }
        const T& operator[](size_t idx) const
        {
            return _instances.at(idx);
        }

        std::vector<T>::iterator begin() // NOLINT(readability-identifier-naming)
        {
            return _instances.begin();
        }
        std::vector<T>::const_iterator begin() const // NOLINT(readability-identifier-naming)
        {
            return _instances.cbegin();
        }
        std::vector<T>::const_iterator cbegin() const // NOLINT(readability-identifier-naming)
        {
            return _instances.cbegin();
        }
        std::vector<T>::iterator end() // NOLINT(readability-identifier-naming)
        {
            return _instances.begin() + _numInstances;
        }
        std::vector<T>::const_iterator end() const // NOLINT(readability-identifier-naming)
        {
            return _instances.cbegin() + _numInstances;
        }
        std::vector<T>::const_iterator cend() const // NOLINT(readability-identifier-naming)
        {
            return _instances.cbegin() + _numInstances;
        }
    };

    struct DrawLineCommand
    {
        ivec4 bounds;
        GLuint colour;
        GLint depth;
    };

    // Per-instance data for images.
    // Layout is packed so the VAO stays within GL_MAX_VERTEX_ATTRIBS (16):
    //   palettes+flags → 1 attrib, depth+zoom → 1 attrib, world depth → 2 attribs.
    struct DrawRectCommand
    {
        ivec4 clip;
        GLint texColourAtlas;
        vec4 texColourBounds;
        GLint texMaskAtlas;
        vec4 texMaskBounds;
        ivec4 palettesFlags; // xyz = remaps, w = flags
        GLuint colour;
        ivec4 bounds;
        vec2 depthZoom; // x = painter depth, y = zoom
        // World-depth solve inputs; layout per mode in OpenGLDrawingContext::ApplyWorldDepthToCommand.
        vec4 world0;
        vec4 world1;

        enum
        {
            FLAG_NO_TEXTURE = (1u << 2u),
            FLAG_MASK = (1u << 3u),
            FLAG_CROSS_HATCH = (1u << 4u),
            FLAG_TTF_TEXT = (1u << 5u),
            // Writes only the world-depth attachment (as "no depth here"), never colour.
            FLAG_WORLD_DEPTH_ONLY = (1u << 6u),
            // bits 8 to 16 used to store hinting threshold.
            FLAG_TTF_HINTING_THRESHOLD_MASK = 0xff00
        };
    };

    using LineCommandBatch = CommandBatch<DrawLineCommand>;
    using RectCommandBatch = CommandBatch<DrawRectCommand>;
} // namespace OpenRCT2::Ui
