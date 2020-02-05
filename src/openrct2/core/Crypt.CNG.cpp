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

#ifdef __USE_CNG__

#include "Crypt.h"
#include "../platform/Platform2.h"
#include "IStream.hpp"
#include <stdexcept>
#include <sstream>
#include <string>
#include <tuple>

// clang-format off
// CNG: Cryptography API: Next Generation (CNG)
//      available in Windows Vista onwards.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>
#include <bcrypt.h>
#include <ncrypt.h>
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
// clang-format on

using namespace Crypt;

static void CngThrowOnBadStatus(const std::string_view& name, NTSTATUS status)
{
    if (!NT_SUCCESS(status))
    {
        throw std::runtime_error(std::string(name) + " failed: " + std::to_string(status));
    }
}

static void ThrowBadAllocOnNull(const void * ptr)
{
    if (ptr == nullptr)
    {
        throw std::bad_alloc();
    }
}

template<typename TBase>
class CngHashAlgorithm final : public TBase
{
private:
    const wchar_t * _algName;
    BCRYPT_ALG_HANDLE _hAlg{};
    BCRYPT_HASH_HANDLE _hHash{};
    PBYTE _pbHashObject{};
    bool _reusable{};

public:
    CngHashAlgorithm(const wchar_t * algName)
    {
        // BCRYPT_HASH_REUSABLE_FLAG only available from Windows 8
        _algName = algName;
        _reusable = Platform::IsOSVersionAtLeast(6, 2, 0);
        Initialise();
    }

    ~CngHashAlgorithm()
    {
        Dispose();
    }

    TBase * Clear() override
    {
        if (_reusable)
        {
            // Finishing the current digest clears the state ready for a new digest
            Finish();
        }
        else
        {
            Dispose();
            Initialise();
        }
        return this;
    }

    TBase * Update(const void * data, size_t dataLen) override
    {
        auto status = BCryptHashData(_hHash, (PBYTE)data, (ULONG)dataLen, 0);
        CngThrowOnBadStatus("BCryptHashData", status);
        return this;
    }

    typename TBase::Result Finish() override
    {
        typename TBase::Result result;
        auto status = BCryptFinishHash(_hHash, result.data(), (ULONG)result.size(), 0);
        CngThrowOnBadStatus("BCryptFinishHash", status);
        return result;
    }

private:
    void Initialise()
    {
        auto flags = _reusable ? BCRYPT_HASH_REUSABLE_FLAG : 0;
        auto status = BCryptOpenAlgorithmProvider(&_hAlg, _algName, nullptr, flags);
        CngThrowOnBadStatus("BCryptOpenAlgorithmProvider", status);

        // Calculate the size of the buffer to hold the hash object
        DWORD cbHashObject{};
        DWORD cbData{};
        status = BCryptGetProperty(_hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbHashObject, sizeof(DWORD), &cbData, 0);
        CngThrowOnBadStatus("BCryptGetProperty", status);

        // Create a hash
        _pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
        ThrowBadAllocOnNull(_pbHashObject);
        status = BCryptCreateHash(_hAlg, &_hHash, _pbHashObject, cbHashObject, nullptr, 0, 0);
        CngThrowOnBadStatus("BCryptCreateHash", status);
    }

    void Dispose()
    {
        BCryptCloseAlgorithmProvider(_hAlg, 0);
        BCryptDestroyHash(_hHash);
        HeapFree(GetProcessHeap(), 0, _pbHashObject);

        _hAlg = {};
        _hHash = {};
        _pbHashObject = {};
    }
};

class DerReader
{
private:
    ivstream<uint8_t> _stream;

    template<typename T>
    T Read(std::istream& stream)
    {
        T value;
        stream.read((char*)&value, sizeof(T));
        return value;
    }

    template<typename T>
    std::vector<T> Read(std::istream& stream, size_t count)
    {
        std::vector<T> values(count);
        stream.read((char*)values.data(), sizeof(T) * count);
        return values;
    }

    int ReadTag(std::istream& stream)
    {
        auto a = Read<uint8_t>(stream);
        // auto tagClass = a >> 6;
        // auto tagConstructed = ((a & 0x20) != 0);
        auto tagNumber = a & 0x1F;
        if (tagNumber == 0x1F)
        {
            throw std::runtime_error("Unsupported DER tag");
        }
        return tagNumber;
    }

    int ReadLength(std::istream& stream)
    {
        auto a = Read<uint8_t>(stream);
        auto len = a & 0x7F;
        if (len == a)
        {
            return len;
        }
        if (len > 6)
        {
            throw std::runtime_error("Length over 48 bits not supported at this position");
        }
        if (len == 0)
        {
            throw std::runtime_error("Unknown length");
        }
        auto result = 0;
        for (auto i = 0; i < len; i++)
        {
            result = (result << 8) + Read<uint8_t>(stream);
        }
        return result;
    }

public:
    DerReader(const std::vector<uint8_t>& data)
        : _stream(data)
    {
    }

    void ReadSequenceHeader()
    {
        auto a = Read<uint16_t>(_stream);
        if (a == 0x8130)
        {
            Read<uint8_t>(_stream);
        }
        else if (a == 0x8230)
        {
            Read<uint16_t>(_stream);
        }
        else
        {
            throw std::runtime_error("Invalid DER code");
        }
    }

    std::vector<uint8_t> ReadInteger()
    {
        auto t = ReadTag(_stream);
        if (t != 2)
        {
            throw std::runtime_error("Expected INTEGER");
        }
        auto len = ReadLength(_stream);
        auto result = Read<uint8_t>(_stream, len);

        // auto v = result[0];
        // auto neg = (v > 127);
        // auto pad = neg ? 255 : 0;
        // for (size_t i = 0; i < result.size(); i++)
        // {
        //     if (result[i] != pad)
        //     {
        //         result.erase(result.begin(), result.begin() + i);
        //         break;
        //     }
        // }
        return result;
    }
};

class DerWriter
{
private:
    std::vector<uint8_t> _buffer;

public:
    void WriteSequenceHeader()
    {
        _buffer.push_back(0x30);
        _buffer.push_back(0x81);
        _buffer.push_back(0x89);
    }

    void WriteInteger(const std::vector<uint8_t>& data)
    {
        _buffer.push_back(0x02);
        if (data.size() < 128)
        {
            _buffer.push_back((uint8_t)data.size());
        }
        else if (data.size() <= std::numeric_limits<uint8_t>().max())
        {
            _buffer.push_back(0b10000001);
            _buffer.push_back((uint8_t)data.size());
        }
        else if (data.size() <= std::numeric_limits<uint16_t>().max())
        {
            _buffer.push_back(0b10000010);
            _buffer.push_back((data.size() >> 8) & 0xFF);
            _buffer.push_back(data.size() & 0xFF);
        }
        _buffer.insert(_buffer.end(), data.begin(), data.end());
    }

    std::vector<uint8_t>&& Complete()
    {
        return std::move(_buffer);
    }
};

class CngRsaKey final : public RsaKey
{
private:
    struct RsaKeyParams
    {
        std::vector<uint8_t> Modulus;
        std::vector<uint8_t> Exponent;
        std::vector<uint8_t> Prime1;
        std::vector<uint8_t> Prime2;
        std::vector<uint8_t> Exponent1;
        std::vector<uint8_t> Exponent2;
        std::vector<uint8_t> Coefficient;
        std::vector<uint8_t> PrivateExponent;
    };

public:
    NCRYPT_KEY_HANDLE GetKeyHandle() const { return _hKey; }

    ~CngRsaKey()
    {
        NCryptFreeObject(_hKey);
    }

    void SetPrivate(const std::string_view& pem) override
    {
        auto der = ReadPEM(pem, SZ_PRIVATE_BEGIN_TOKEN, SZ_PRIVATE_END_TOKEN);
        DerReader derReader(der);
        RsaKeyParams params;
        derReader.ReadSequenceHeader();
        derReader.ReadInteger();
        params.Modulus = derReader.ReadInteger();
        params.Exponent = derReader.ReadInteger();
        derReader.ReadInteger();
        params.Prime1 = derReader.ReadInteger();
        params.Prime2 = derReader.ReadInteger();
        _hKey = ImportKey(params);
    }

    void SetPublic(const std::string_view& pem) override
    {
        auto der = ReadPEM(pem, SZ_PUBLIC_BEGIN_TOKEN, SZ_PUBLIC_END_TOKEN);
        DerReader derReader(der);
        RsaKeyParams params;
        derReader.ReadSequenceHeader();
        params.Modulus = derReader.ReadInteger();
        params.Exponent = derReader.ReadInteger();
        _hKey = ImportKey(params);
    }

    std::string GetPrivate() override
    {
        auto params = ExportKey(false);
        DerWriter derWriter;
        derWriter.WriteSequenceHeader();
        derWriter.WriteInteger({});
        derWriter.WriteInteger(params.Modulus);
        derWriter.WriteInteger(params.Exponent);
        derWriter.WriteInteger(params.PrivateExponent);
        derWriter.WriteInteger(params.Prime1);
        derWriter.WriteInteger(params.Prime2);
        derWriter.WriteInteger(params.Exponent1);
        derWriter.WriteInteger(params.Exponent2);
        derWriter.WriteInteger(params.Coefficient);
        auto derBytes = derWriter.Complete();
        auto b64 = EncodeBase64(derBytes);

        std::ostringstream sb;
        sb << std::string(SZ_PRIVATE_BEGIN_TOKEN) << std::endl;
        sb << b64;
        sb << std::string(SZ_PRIVATE_END_TOKEN) << std::endl;
        return sb.str();
    }

    std::string GetPublic() override
    {
        auto params = ExportKey(true);
        DerWriter derWriter;
        derWriter.WriteSequenceHeader();
        derWriter.WriteInteger(params.Modulus);
        derWriter.WriteInteger(params.Exponent);
        auto derBytes = derWriter.Complete();
        auto b64 = EncodeBase64(derBytes);

        std::ostringstream sb;
        sb << std::string(SZ_PUBLIC_BEGIN_TOKEN) << std::endl;
        sb << b64;
        sb << std::string(SZ_PUBLIC_END_TOKEN) << std::endl;
        return sb.str();
    }

    void Generate() override
    {
    }

private:
    static constexpr std::string_view SZ_PUBLIC_BEGIN_TOKEN = "-----BEGIN RSA PUBLIC KEY-----";
    static constexpr std::string_view SZ_PUBLIC_END_TOKEN = "-----END RSA PUBLIC KEY-----";
    static constexpr std::string_view SZ_PRIVATE_BEGIN_TOKEN = "-----BEGIN RSA PRIVATE KEY-----";
    static constexpr std::string_view SZ_PRIVATE_END_TOKEN = "-----END RSA PRIVATE KEY-----";

    NCRYPT_KEY_HANDLE _hKey{};

    static std::vector<uint8_t> ReadPEM(const std::string_view& pem, const std::string_view& beginToken, const std::string_view& endToken)
    {
        auto beginPos = pem.find(beginToken);
        auto endPos = pem.find(endToken);
        if (beginPos != std::string::npos && endPos != std::string::npos)
        {
            beginPos += beginToken.size();
            auto code = Trim(pem.substr(beginPos, endPos - beginPos));
            return DecodeBase64(code);
        }
        throw std::runtime_error("Invalid PEM file");
    }

    static std::string_view Trim(std::string_view input)
    {
        for (size_t i = 0; i < input.size(); i++)
        {
            if (input[i] >= '!')
            {
                input.remove_prefix(i);
                break;
            }
        }
        for (size_t i = input.size() - 1; i > 0; i--)
        {
            if (input[i] >= '!')
            {
                input = input.substr(0, i + 1);
                break;
            }
        }
        return input;
    }

    static std::string EncodeBase64(const std::vector<uint8_t>& input)
    {
        DWORD flags = CRYPT_STRING_BASE64 | CRYPT_STRING_NOCR;
        DWORD chString{};
        if (!CryptBinaryToStringA(input.data(), (DWORD)input.size(), flags, NULL, &chString))
        {
            throw std::runtime_error("CryptBinaryToStringA failed");
        }
        std::string result(chString, 0);
        if (!CryptBinaryToStringA(input.data(), (DWORD)input.size(), flags, result.data(), &chString))
        {
            throw std::runtime_error("CryptBinaryToStringA failed");
        }

        // CryptBinaryToStringA returns length that includes null terminator
        result.resize(result.size() - 1);

        return result;
    }

    static std::vector<uint8_t> DecodeBase64(const std::string_view& input)
    {
        DWORD cbBinary{};
        if (!CryptStringToBinaryA(input.data(), (DWORD)input.size(), CRYPT_STRING_BASE64, NULL, &cbBinary, NULL, NULL))
        {
            throw std::runtime_error("CryptStringToBinaryA failed");
        }
        std::vector<uint8_t> result(cbBinary);
        if (!CryptStringToBinaryA(input.data(), (DWORD)input.size(), CRYPT_STRING_BASE64, result.data(), &cbBinary, NULL, NULL))
        {
            throw std::runtime_error("CryptStringToBinaryA failed");
        }
        return result;
    }

    static NCRYPT_KEY_HANDLE ImportKey(const RsaKeyParams& params)
    {
        bool isPublic = params.Prime1.size() == 0;
        auto blobType = isPublic ? BCRYPT_RSAPUBLIC_BLOB : BCRYPT_RSAPRIVATE_BLOB;

        BCRYPT_RSAKEY_BLOB header{};
        header.Magic = isPublic ? BCRYPT_RSAPUBLIC_MAGIC : BCRYPT_RSAPRIVATE_MAGIC;
        header.BitLength = (ULONG)(params.Modulus.size() * 8);
        header.cbPublicExp = (ULONG)params.Exponent.size();
        header.cbModulus = (ULONG)params.Modulus.size();
        header.cbPrime1 = (ULONG)params.Prime1.size();
        header.cbPrime2 = (ULONG)params.Prime2.size();

        std::vector<uint8_t> blob;
        blob.insert(blob.end(), (uint8_t*)&header, (uint8_t*)(&header + 1));
        blob.insert(blob.end(), params.Exponent.begin(), params.Exponent.end());
        blob.insert(blob.end(), params.Modulus.begin(), params.Modulus.end());
        blob.insert(blob.end(), params.Prime1.begin(), params.Prime1.end());
        blob.insert(blob.end(), params.Prime2.begin(), params.Prime2.end());

        NCRYPT_PROV_HANDLE hProv{};
        NCRYPT_KEY_HANDLE hKey{};
        auto status = NCryptOpenStorageProvider(&hProv, MS_KEY_STORAGE_PROVIDER, 0);
        CngThrowOnBadStatus("NCryptOpenStorageProvider", status);
        status = NCryptImportKey(hProv, NULL, blobType, NULL, &hKey, (PBYTE)blob.data(), (DWORD)blob.size(), 0);
        NCryptFreeObject(hProv);
        CngThrowOnBadStatus("NCryptImportKey", status);
        return hKey;
    }

    RsaKeyParams ExportKey(bool onlyPublic)
    {
        auto blobType = onlyPublic ? BCRYPT_RSAPUBLIC_BLOB : BCRYPT_RSAPRIVATE_BLOB;

        std::vector<uint8_t> output;
        NCRYPT_PROV_HANDLE hProv{};
        try
        {
            auto status = NCryptOpenStorageProvider(&hProv, MS_KEY_STORAGE_PROVIDER, 0);
            CngThrowOnBadStatus("NCryptOpenStorageProvider", status);
            DWORD cbOutput{};
            status = NCryptExportKey(_hKey, NULL, blobType, NULL, NULL, 0, &cbOutput, 0);
            CngThrowOnBadStatus("NCryptExportKey", status);
            output = std::vector<uint8_t>(cbOutput);
            status = NCryptExportKey(_hKey, NULL, blobType, NULL, output.data(), cbOutput, &cbOutput, 0);
            CngThrowOnBadStatus("NCryptExportKey", status);
            NCryptFreeObject(hProv);
        }
        catch (const std::exception&)
        {
            NCryptFreeObject(hProv);
            throw;
        }

        size_t offset{};
        RsaKeyParams params;
        const auto& header = *((BCRYPT_RSAKEY_BLOB*)output.data());
        ReadBytes(output, offset, sizeof(BCRYPT_RSAKEY_BLOB));
        params.Exponent = ReadBytes(output, offset, header.cbPublicExp);
        params.Modulus = ReadBytes(output, offset, header.cbModulus);
        params.Prime1 = ReadBytes(output, offset, header.cbPrime1);
        params.Prime2 = ReadBytes(output, offset, header.cbPrime2);
        if (!onlyPublic)
        {
            params.Exponent1 = ReadBytes(output, offset, header.cbPrime1);
            params.Exponent2 = ReadBytes(output, offset, header.cbPrime2);
            params.Coefficient = ReadBytes(output, offset, header.cbPrime1);
            params.PrivateExponent = ReadBytes(output, offset, header.cbModulus);
        }
        return params;
    }

    static std::vector<uint8_t> ReadBytes(std::vector<uint8_t>& src, size_t& offset, size_t length)
    {
        std::vector<uint8_t> result;
        result.insert(result.end(), src.begin() + offset, src.begin() + offset + length);
        offset += length;
        return result;
    }
};

class CngRsaAlgorithm final : public RsaAlgorithm
{
public:
    std::vector<uint8_t> SignData(const RsaKey& key, const void * data, size_t dataLen) override
    {
        auto hKey = static_cast<const CngRsaKey&>(key).GetKeyHandle();
        auto [cbHash, pbHash] = HashData(data, dataLen);
        auto [cbSignature, pbSignature] = std::tuple<DWORD, PBYTE>();
        try
        {
            BCRYPT_PKCS1_PADDING_INFO paddingInfo{ BCRYPT_SHA256_ALGORITHM };
            auto status = NCryptSignHash(hKey, &paddingInfo, pbHash, cbHash, NULL, 0, &cbSignature, BCRYPT_PAD_PKCS1);
            CngThrowOnBadStatus("NCryptSignHash", status);
            pbSignature = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbSignature);
            ThrowBadAllocOnNull(pbSignature);
            status = NCryptSignHash(hKey, &paddingInfo, pbHash, cbHash, pbSignature, cbSignature, &cbSignature, BCRYPT_PAD_PKCS1);
            CngThrowOnBadStatus("NCryptSignHash", status);

            auto result = std::vector<uint8_t>(pbSignature, pbSignature + cbSignature);
            HeapFree(GetProcessHeap(), 0, pbSignature);
            return result;
        }
        catch (std::exception&)
        {
            HeapFree(GetProcessHeap(), 0, pbHash);
            HeapFree(GetProcessHeap(), 0, pbSignature);
            throw;
        }
    }

    bool VerifyData(const RsaKey& key, const void * data, size_t dataLen, const void * sig, size_t sigLen) override
    {
        auto hKey = static_cast<const CngRsaKey&>(key).GetKeyHandle();
        auto [cbHash, pbHash] = HashData(data, dataLen);
        auto [cbSignature, pbSignature] = ToHeap(sig, sigLen);

        BCRYPT_PKCS1_PADDING_INFO paddingInfo { BCRYPT_SHA256_ALGORITHM };
        auto status = NCryptVerifySignature(hKey, &paddingInfo, pbHash, cbHash, pbSignature, cbSignature, BCRYPT_PAD_PKCS1);
        HeapFree(GetProcessHeap(), 0, pbSignature);
        return status == ERROR_SUCCESS;
    }

private:
    static std::tuple<DWORD, PBYTE> HashData(const void * data, size_t dataLen)
    {
        auto hash = Crypt::SHA256(data, dataLen);
        return ToHeap(hash.data(), hash.size());
    }

    static std::tuple<DWORD, PBYTE> ToHeap(const void * data, size_t dataLen)
    {
        auto cbHash = (DWORD)dataLen;
        auto pbHash = (PBYTE)HeapAlloc(GetProcessHeap(), 0, dataLen);
        ThrowBadAllocOnNull(pbHash);
        std::memcpy(pbHash, data, dataLen);
        return std::make_tuple(cbHash, pbHash);
    }
};

namespace Crypt
{
    std::unique_ptr<Sha1Algorithm> CreateSHA1()
    {
        return std::make_unique<CngHashAlgorithm<Sha1Algorithm>>(BCRYPT_SHA1_ALGORITHM);
    }

    std::unique_ptr<Sha256Algorithm> CreateSHA256()
    {
        return std::make_unique<CngHashAlgorithm<Sha256Algorithm>>(BCRYPT_SHA256_ALGORITHM);
    }

    std::unique_ptr<RsaAlgorithm> CreateRSA()
    {
        return std::make_unique<CngRsaAlgorithm>();
    }

    std::unique_ptr<RsaKey> CreateRSAKey()
    {
        return std::make_unique<CngRsaKey>();
    }
}

#endif
