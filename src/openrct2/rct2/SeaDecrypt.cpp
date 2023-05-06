/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"
#include "../core/File.h"
#include "../core/Numerics.hpp"
#include "../core/Path.hpp"
#include "RCT2.h"

#include <cstdint>
#include <cstring>
#include <memory>
#include <string_view>

constexpr int32_t MASK_SIZE = 0x1000;

struct EncryptionKey
{
    uint32_t Seed0{};
    uint32_t Seed1{};
};

static EncryptionKey GetEncryptionKey(std::string_view fileName)
{
    auto fileNameLen = static_cast<int32_t>(fileName.size());
    uint32_t s0 = 0;
    for (int i = fileNameLen - 1; i >= 0; i--)
    {
        s0 = (s0 + (s0 << 5)) ^ fileName[i];
    }

    uint32_t s1 = 0;
    for (int i = 0; i < fileNameLen; i++)
    {
        s1 = (s1 + (s1 << 5)) ^ fileName[i];
    }

    return EncryptionKey{ s0, s1 };
}

static std::vector<uint8_t> CreateMask(const EncryptionKey& key)
{
    std::vector<uint8_t> result;
    result.resize(MASK_SIZE);
    uint32_t seed0 = key.Seed0;
    uint32_t seed1 = key.Seed1;
    for (size_t i = 0; i < MASK_SIZE; i += 4)
    {
        uint32_t s0 = seed0;
        uint32_t s1 = seed1 ^ 0xF7654321;
        seed0 = Numerics::rol32(s1, 25) + s0;
        seed1 = Numerics::rol32(s0, 29);
        result[i + 0] = (s0 >> 3) & 0xFF;
        result[i + 1] = (s0 >> 11) & 0xFF;
        result[i + 2] = (s0 >> 19) & 0xFF;
        result[i + 3] = (seed1 >> 24) & 0xFF;
    }
    return result;
}

static void Decrypt(std::vector<uint8_t>& data, const EncryptionKey& key)
{
    auto mask = CreateMask(key);
    uint32_t b = 0;
    uint32_t c = 0;
    for (size_t i = 0; i < data.size(); i++)
    {
        auto a = b % MASK_SIZE;
        c = c % MASK_SIZE;
        b = (a + 1) % MASK_SIZE;

        data[i] = (((data[i] - mask[b]) ^ mask[c]) + mask[a]) & 0xFF;

        c += 3;
        b = a + 7;
    }
}

std::vector<uint8_t> DecryptSea(const fs::path& path)
{
    auto key = GetEncryptionKey(path.filename().u8string());
    auto data = File::ReadAllBytes(path.u8string());

    // Last 4 bytes is the checksum
    size_t inputSize = data.size() - 4;
    uint32_t checksum;
    std::memcpy(&checksum, data.data() + inputSize, sizeof(checksum));
    data.resize(inputSize);

    Decrypt(data, key);
    return data;
}
