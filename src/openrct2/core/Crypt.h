/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <array>
#include <memory>
#include <string_view>
#include <vector>

namespace Crypt
{
    template<size_t TLength> class HashAlgorithm
    {
    public:
        using Result = std::array<uint8_t, TLength>;

        virtual ~HashAlgorithm() = default;
        virtual HashAlgorithm* Clear() = 0;
        virtual HashAlgorithm* Update(const void* data, size_t dataLen) = 0;
        virtual Result Finish() = 0;
    };

    class RsaKey
    {
    public:
        virtual ~RsaKey() = default;
        virtual void Generate() = 0;
        virtual void SetPrivate(std::string_view pem) = 0;
        virtual void SetPublic(std::string_view pem) = 0;
        virtual std::string GetPrivate() = 0;
        virtual std::string GetPublic() = 0;
    };

    class RsaAlgorithm
    {
    public:
        virtual ~RsaAlgorithm() = default;
        virtual std::vector<uint8_t> SignData(const RsaKey& key, const void* data, size_t dataLen) = 0;
        virtual bool VerifyData(const RsaKey& key, const void* data, size_t dataLen, const void* sig, size_t sigLen) = 0;
    };

    using Sha1Algorithm = HashAlgorithm<20>;
    using Sha256Algorithm = HashAlgorithm<32>;
    using FNV1aAlgorithm = HashAlgorithm<8>;

    // Factories
    [[nodiscard]] std::unique_ptr<FNV1aAlgorithm> CreateFNV1a();
    [[nodiscard]] std::unique_ptr<Sha1Algorithm> CreateSHA1();
    [[nodiscard]] std::unique_ptr<Sha256Algorithm> CreateSHA256();
    [[nodiscard]] std::unique_ptr<RsaAlgorithm> CreateRSA();
    [[nodiscard]] std::unique_ptr<RsaKey> CreateRSAKey();

    inline FNV1aAlgorithm::Result FNV1a(const void* data, size_t dataLen)
    {
        return CreateFNV1a()->Update(data, dataLen)->Finish();
    }

    inline Sha1Algorithm::Result SHA1(const void* data, size_t dataLen)
    {
        return CreateSHA1()->Update(data, dataLen)->Finish();
    }

    inline Sha256Algorithm::Result SHA256(const void* data, size_t dataLen)
    {
        return CreateSHA256()->Update(data, dataLen)->Finish();
    }
} // namespace Crypt
