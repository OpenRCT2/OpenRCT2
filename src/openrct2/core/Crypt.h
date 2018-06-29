/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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
    template<size_t TLength>
    class HashAlgorithm
    {
    public:
        typedef std::array<uint8_t, TLength> Result;

        virtual ~HashAlgorithm() = default;
        virtual HashAlgorithm * Clear() = 0;
        virtual HashAlgorithm * Update(const void * data, size_t dataLen) = 0;
        virtual Result Finish() = 0;
    };

    class RsaKey
    {
    public:
        virtual ~RsaKey() = default;
        virtual void Generate() = 0;
        virtual void SetPrivate(const std::string_view& pem) = 0;
        virtual void SetPublic(const std::string_view& pem) = 0;
        virtual std::string GetPrivate() = 0;
        virtual std::string GetPublic() = 0;
    };

    class RsaAlgorithm
    {
    public:
        virtual ~RsaAlgorithm() = default;
        virtual std::vector<uint8_t> SignData(const RsaKey& key, const void * data, size_t dataLen) = 0;
        virtual bool VerifyData(const RsaKey& key, const void * data, size_t dataLen, const void * sig, size_t sigLen) = 0;
    };

    using Sha1Algorithm = HashAlgorithm<20>;
    using Sha256Algorithm = HashAlgorithm<32>;

    // Factories
    std::unique_ptr<Sha1Algorithm> CreateSHA1();
    std::unique_ptr<Sha256Algorithm> CreateSHA256();
    std::unique_ptr<RsaAlgorithm> CreateRSA();
    std::unique_ptr<RsaKey> CreateRSAKey();

    inline Sha1Algorithm::Result SHA1(const void * data, size_t dataLen)
    {
        return CreateSHA1()
            ->Update(data, dataLen)
            ->Finish();
    }

    inline Sha256Algorithm::Result SHA256(const void * data, size_t dataLen)
    {
        return CreateSHA256()
            ->Update(data, dataLen)
            ->Finish();
    }
}
