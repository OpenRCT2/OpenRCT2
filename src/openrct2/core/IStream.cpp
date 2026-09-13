/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "IStream.hpp"

#include "StreamBuffer.hpp"
#include "String.hpp"

#include <vector>

namespace OpenRCT2
{
    constexpr size_t kIStreamCopyBufferLength = 16 * 1024;

    void IStream::CopyFromStream(IStream& stream, uint64_t length)
    {
        StreamReadBuffer buffer(stream, length, kIStreamCopyBufferLength);
        while (buffer)
        {
            auto block = buffer.ReadBlock(stream);
            this->Write(block.first, block.second);
        }
    }

    std::string IStream::ReadString()
    {
        std::string result;
        uint8_t ch;
        while ((ch = ReadValue<uint8_t>()) != 0)
        {
            result.push_back(ch);
        }
        return result;
    }

    void IStream::WriteString(std::string_view str)
    {
        // if the string contains any null characters, then stop the write at the first one
        str = String::toStringView(str.data(), str.size());
        Write(str.data(), str.size());
        WriteValue<uint8_t>(0);
    }
} // namespace OpenRCT2
