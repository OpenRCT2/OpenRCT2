/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/IStream.hpp"
#include "../core/SawyerCoding.h"
#include "SawyerChunk.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace OpenRCT2
{
    struct IStream;

    class SawyerChunkException : public IOException
    {
    public:
        explicit SawyerChunkException(const char* message)
            : IOException(message)
        {
        }
        explicit SawyerChunkException(const std::string& message)
            : IOException(message)
        {
        }
    };

    /**
     * Reads sawyer encoding chunks from a data stream. This can be used to read
     * SC6, SV6 and RCT2 objects. persistentChunks is a hint to the reader that the chunk will be preserved,
     * and thus the chunk memory should be shrunk.
     */
    class SawyerChunkReader final
    {
    private:
        OpenRCT2::IStream* const _stream = nullptr;

    public:
        explicit SawyerChunkReader(OpenRCT2::IStream* stream);

        /**
         * Skips the next chunk in the stream without decoding or reading its data
         * into RAM.
         */
        void SkipChunk();

        /**
         * Reads the next chunk from the stream.
         */
        [[nodiscard]] std::shared_ptr<SawyerChunk> ReadChunk();

        /**
         * As above but for chunks without a header
         */
        [[nodiscard]] std::shared_ptr<SawyerChunk> ReadChunkTrack();

        /**
         * Reads the next chunk from the stream and copies it directly to the
         * destination buffer. If the chunk is larger than length, only length
         * is copied. If the chunk is smaller than length, the remaining space
         * is padded with zero.
         * @param dst The destination buffer.
         * @param length The size of the destination buffer.
         */
        void ReadChunk(void* dst, size_t length);

        /**
         * Reads the next chunk from the stream into a buffer returned as the
         * specified type. If the chunk is smaller than the size of the type
         * then the remaining space is padded with zero.
         */
        template<typename T>
        T ReadChunkAs()
        {
            T result;
            ReadChunk(&result, sizeof(result));
            return result;
        }
    };
} // namespace OpenRCT2
