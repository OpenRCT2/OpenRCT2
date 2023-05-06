/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "SawyerChunk.h"

#include <memory>

namespace OpenRCT2
{
    struct IStream;
}

/**
 * Writes sawyer encoding chunks to a data stream. This can be used to write
 * SC6 and SV6 files.
 */
class SawyerChunkWriter final
{
private:
    OpenRCT2::IStream* const _stream = nullptr;

public:
    explicit SawyerChunkWriter(OpenRCT2::IStream* stream);

    /**
     * Writes a chunk to the stream.
     */
    void WriteChunk(const SawyerChunk* chunk);

    /**
     * Writes a chunk to the stream containing the given buffer.
     * @param src The source buffer.
     * @param length The size of the source buffer.
     */
    void WriteChunk(const void* src, size_t length, SAWYER_ENCODING encoding);

    /**
     * Writes a track chunk to the stream containing the given buffer.
     * @param src The source buffer.
     * @param length The size of the source buffer.
     */
    void WriteChunkTrack(const void* src, size_t length);

    /**
     * Writes a chunk to the stream containing the given type.
     */
    template<typename T> void WriteChunk(const T* src, SAWYER_ENCODING encoding)
    {
        WriteChunk(src, sizeof(T), encoding);
    }
};
