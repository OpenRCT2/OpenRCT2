#pragma region Copyright (c) 2018 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

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
