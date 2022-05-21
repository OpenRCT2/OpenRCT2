/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Crypt.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>

using namespace Crypt;

class OpenRCT2FNV1aAlgorithm final : public FNV1aAlgorithm
{
private:
    static constexpr uint64_t Offset = 0xCBF29CE484222325ULL;
    static constexpr uint64_t Prime = 0x00000100000001B3ULL;

    uint64_t _data = Offset;
    uint8_t _rem[8]{};
    size_t _remLen{};

    void ProcessRemainder()
    {
        if (_remLen > 0)
        {
            uint64_t temp{};
            std::memcpy(&temp, _rem, _remLen);
            _data ^= temp;
            _data *= Prime;
            _remLen = 0;
        }
    }

public:
    HashAlgorithm* Clear() override
    {
        _data = Offset;
        return this;
    }

    HashAlgorithm* Update(const void* data, size_t dataLen) override
    {
        if (dataLen == 0)
            return this;

        auto src = reinterpret_cast<const uint64_t*>(data);
        if (_remLen > 0)
        {
            // We have remainder, so fill rest of it with bytes from src
            auto fillLen = sizeof(uint64_t) - _remLen;
            assert(_remLen + fillLen <= sizeof(uint64_t));
            std::memcpy(_rem + _remLen, src, fillLen);
            src = reinterpret_cast<const uint64_t*>(reinterpret_cast<const uint8_t*>(src) + fillLen);
            _remLen += fillLen;
            dataLen -= fillLen;
            ProcessRemainder();
        }

        // Process every block of 8 bytes
        while (dataLen >= sizeof(uint64_t))
        {
            auto temp = *src++;
            _data ^= temp;
            _data *= Prime;
            dataLen -= sizeof(uint64_t);
        }

        // Store the remaining data (< 8 bytes)
        if (dataLen > 0)
        {
            _remLen = dataLen;
            std::memcpy(&_rem, src, dataLen);
        }
        return this;
    }

    Result Finish() override
    {
        ProcessRemainder();

        Result res;
        std::memcpy(res.data(), &_data, sizeof(_data));
        return res;
    }
};

namespace Crypt
{
    std::unique_ptr<FNV1aAlgorithm> CreateFNV1a()
    {
        return std::make_unique<OpenRCT2FNV1aAlgorithm>();
    }
} // namespace Crypt
