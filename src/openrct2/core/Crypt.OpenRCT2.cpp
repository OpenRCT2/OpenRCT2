/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Crypt.h"

#include <cstring>

using namespace Crypt;

// https://github.com/CTrabant/teeny-sha1
static int sha1digest(uint8_t* digest, const uint8_t* data, size_t databytes)
{
#define SHA1ROTATELEFT(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

    uint32_t W[80];
    uint32_t H[] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f = 0;
    uint32_t k = 0;

    uint32_t idx;
    uint32_t lidx;
    uint32_t widx;
    uint32_t didx = 0;

    int32_t wcount;
    uint32_t temp;
    uint64_t databits = (static_cast<uint64_t>(databytes)) * 8;
    auto loopcount = (databytes + 8) / 64 + 1;
    auto tailbytes = 64 * loopcount - databytes;
    uint8_t datatail[128] = { 0 };

    if (!digest)
        return -1;

    if (!data)
        return -1;

    /* Pre-processing of data tail (includes padding to fill out 512-bit chunk):
       Add bit '1' to end of message (big-endian)
       Add 64-bit message length in bits at very end (big-endian) */
    datatail[0] = 0x80;
    datatail[tailbytes - 8] = static_cast<uint8_t>(databits >> 56 & 0xFF);
    datatail[tailbytes - 7] = static_cast<uint8_t>(databits >> 48 & 0xFF);
    datatail[tailbytes - 6] = static_cast<uint8_t>(databits >> 40 & 0xFF);
    datatail[tailbytes - 5] = static_cast<uint8_t>(databits >> 32 & 0xFF);
    datatail[tailbytes - 4] = static_cast<uint8_t>(databits >> 24 & 0xFF);
    datatail[tailbytes - 3] = static_cast<uint8_t>(databits >> 16 & 0xFF);
    datatail[tailbytes - 2] = static_cast<uint8_t>(databits >> 8 & 0xFF);
    datatail[tailbytes - 1] = static_cast<uint8_t>(databits >> 0 & 0xFF);

    /* Process each 512-bit chunk */
    for (lidx = 0; lidx < loopcount; lidx++)
    {
        /* Compute all elements in W */
        std::memset(W, 0, 80 * sizeof(uint32_t));

        /* Break 512-bit chunk into sixteen 32-bit, big endian words */
        for (widx = 0; widx <= 15; widx++)
        {
            wcount = 24;

            /* Copy byte-per byte from specified buffer */
            while (didx < databytes && wcount >= 0)
            {
                W[widx] += (static_cast<uint32_t>(data[didx]) << wcount);
                didx++;
                wcount -= 8;
            }
            /* Fill out W with padding as needed */
            while (wcount >= 0)
            {
                W[widx] += (static_cast<uint32_t>(datatail[didx - databytes]) << wcount);
                didx++;
                wcount -= 8;
            }
        }

        /* Extend the sixteen 32-bit words into eighty 32-bit words, with potential optimization from:
           "Improving the Performance of the Secure Hash Algorithm (SHA-1)" by Max Locktyukhin */
        for (widx = 16; widx <= 31; widx++)
        {
            W[widx] = SHA1ROTATELEFT((W[widx - 3] ^ W[widx - 8] ^ W[widx - 14] ^ W[widx - 16]), 1);
        }
        for (widx = 32; widx <= 79; widx++)
        {
            W[widx] = SHA1ROTATELEFT((W[widx - 6] ^ W[widx - 16] ^ W[widx - 28] ^ W[widx - 32]), 2);
        }

        /* Main loop */
        a = H[0];
        b = H[1];
        c = H[2];
        d = H[3];
        e = H[4];

        for (idx = 0; idx <= 79; idx++)
        {
            if (idx <= 19)
            {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            }
            else if (idx >= 20 && idx <= 39)
            {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            }
            else if (idx >= 40 && idx <= 59)
            {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            }
            else if (idx >= 60 && idx <= 79)
            {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }
            temp = SHA1ROTATELEFT(a, 5) + f + e + k + W[idx];
            e = d;
            d = c;
            c = SHA1ROTATELEFT(b, 30);
            b = a;
            a = temp;
        }

        H[0] += a;
        H[1] += b;
        H[2] += c;
        H[3] += d;
        H[4] += e;
    }

    /* Store binary digest in supplied buffer */
    if (digest)
    {
        for (idx = 0; idx < 5; idx++)
        {
            digest[idx * 4 + 0] = static_cast<uint8_t>(H[idx] >> 24);
            digest[idx * 4 + 1] = static_cast<uint8_t>(H[idx] >> 16);
            digest[idx * 4 + 2] = static_cast<uint8_t>(H[idx] >> 8);
            digest[idx * 4 + 3] = static_cast<uint8_t>(H[idx]);
        }
    }
    return 0;
}

class OpenRCT2Sha1Algorithm : public Sha1Algorithm
{
private:
    std::vector<uint8_t> _data;

public:
    HashAlgorithm* Clear() override
    {
        _data = {};
        return this;
    }

    HashAlgorithm* Update(const void* data, size_t dataLen) override
    {
        auto src = reinterpret_cast<const uint8_t*>(data);
        _data.insert(_data.end(), src, src + dataLen);
        return this;
    }

    Result Finish() override
    {
        std::array<uint8_t, 20> digest{};
        sha1digest(digest.data(), _data.data(), _data.size());
        _data = {};
        return digest;
    }
};

class OpenRCT2FNV1aAlgorithm : public FNV1aAlgorithm
{
private:
    static constexpr uint64_t Offset = 0xcbf29ce484222325ULL;
    static constexpr uint64_t Prime = 0x00000100000001B3ULL;

    uint64_t _data = Offset;

public:
    HashAlgorithm* Clear() override
    {
        _data = Offset;
        return this;
    }

    HashAlgorithm* Update(const void* data, size_t dataLen) override
    {
        // 64b aligned updates.
        for (size_t i = 0; i < dataLen; i += sizeof(uint64_t))
        {
            const auto maxLen = std::min(sizeof(uint64_t), dataLen - i);

            uint64_t temp{};
            std::memcpy(&temp, reinterpret_cast<const std::byte*>(data) + i, maxLen);

            _data ^= temp;
            _data *= Prime;
        }
        return this;
    }

    Result Finish() override
    {
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

    std::unique_ptr<Sha1Algorithm> CreateSHA1()
    {
        return std::make_unique<OpenRCT2Sha1Algorithm>();
    }

} // namespace Crypt
