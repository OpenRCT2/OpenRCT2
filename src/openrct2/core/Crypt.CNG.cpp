/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if !defined(DISABLE_NETWORK) && defined(_WIN32)

#    include "Crypt.h"

#    include "../platform/Platform.h"
#    include "IStream.hpp"

#    include <iomanip>
#    include <sstream>
#    include <stdexcept>
#    include <string>
#    include <tuple>

// clang-format off
// CNG: Cryptography API: Next Generation (CNG)
//      available in Windows Vista onwards.
#include <windows.h>
#include <wincrypt.h>
#include <bcrypt.h>
constexpr bool NT_SUCCESS(NTSTATUS status) {return status >= 0;}
// clang-format on

using namespace Crypt;

static void CngThrowOnBadStatus(std::string_view name, NTSTATUS status)
{
    if (!NT_SUCCESS(status))
    {
        std::stringstream stream;
        stream << "0x" << std::setfill('0') << std::setw(8) << std::hex << status;
        throw std::runtime_error(std::string(name) + " failed: " + stream.str());
    }
}

static void ThrowBadAllocOnNull(const void* ptr)
{
    if (ptr == nullptr)
    {
        throw std::bad_alloc();
    }
}

template<typename TBase> class CngHashAlgorithm final : public TBase
{
private:
    const wchar_t* _algName;
    BCRYPT_ALG_HANDLE _hAlg{};
    BCRYPT_HASH_HANDLE _hHash{};
    PBYTE _pbHashObject{};
    bool _reusable{};

public:
    CngHashAlgorithm(const wchar_t* algName)
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

    TBase* Clear() override
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

    TBase* Update(const void* data, size_t dataLen) override
    {
        auto status = BCryptHashData(_hHash, reinterpret_cast<PBYTE>(const_cast<void*>(data)), static_cast<ULONG>(dataLen), 0);
        CngThrowOnBadStatus("BCryptHashData", status);
        return this;
    }

    typename TBase::Result Finish() override
    {
        typename TBase::Result result;
        auto status = BCryptFinishHash(_hHash, result.data(), static_cast<ULONG>(result.size()), 0);
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
        status = BCryptGetProperty(
            _hAlg, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PBYTE>(&cbHashObject), sizeof(DWORD), &cbData, 0);
        CngThrowOnBadStatus("BCryptGetProperty", status);

        // Create a hash
        _pbHashObject = reinterpret_cast<PBYTE>(HeapAlloc(GetProcessHeap(), 0, cbHashObject));
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

    template<typename T> T Read(std::istream& stream)
    {
        T value;
        stream.read(reinterpret_cast<char*>(&value), sizeof(T));
        return value;
    }

    template<typename T> std::vector<T> Read(std::istream& stream, size_t count)
    {
        std::vector<T> values(count);
        stream.read(reinterpret_cast<char*>(values.data()), sizeof(T) * count);
        return values;
    }

    int ReadTag(std::istream& stream)
    {
        auto a = Read<uint8_t>(stream);
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

        auto v = result[0];
        auto neg = (v > 127);
        auto pad = neg ? 255 : 0;
        for (size_t i = 0; i < result.size(); i++)
        {
            if (result[i] != pad)
            {
                result.erase(result.begin(), result.begin() + i);
                break;
            }
        }
        return result;
    }
};

class DerWriter
{
private:
    std::vector<uint8_t> _buffer;

public:
    void WriteSequenceHeader(size_t len)
    {
        _buffer.push_back(0x30);
        WriteCompressedNumber(len);
    }

    void WriteInteger(const std::vector<uint8_t>& data)
    {
        _buffer.push_back(0x02);

        size_t dataLen = data.size();
        if (dataLen > 0 && data[0] > 127)
        {
            // Prepend a zero to number so it isn't treated as negative
            WriteCompressedNumber(dataLen + 1);
            _buffer.push_back(0);
            _buffer.insert(_buffer.end(), data.begin(), data.end());
        }
        else
        {
            WriteCompressedNumber(dataLen);
            _buffer.insert(_buffer.end(), data.begin(), data.end());
        }
    }

    void WriteCompressedNumber(uint64_t value)
    {
        if (value < 128)
        {
            _buffer.push_back(static_cast<uint8_t>(value));
        }
        else if (value <= std::numeric_limits<uint8_t>().max())
        {
            _buffer.push_back(0b10000001);
            _buffer.push_back(static_cast<uint8_t>(value));
        }
        else if (value <= std::numeric_limits<uint16_t>().max())
        {
            _buffer.push_back(0b10000010);
            _buffer.push_back((value >> 8) & 0xFF);
            _buffer.push_back(value & 0xFF);
        }
    }

    std::vector<uint8_t>&& Complete()
    {
        auto oldBuffer = std::move(_buffer);
        WriteSequenceHeader(oldBuffer.size());
        _buffer.insert(_buffer.end(), oldBuffer.begin(), oldBuffer.end());
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

        ULONG GetMagic() const
        {
            ULONG magic = BCRYPT_RSAPUBLIC_MAGIC;
            if (!Prime1.empty() || !Prime2.empty())
                magic = BCRYPT_RSAPRIVATE_MAGIC;
            if (!Exponent1.empty())
                magic = BCRYPT_RSAFULLPRIVATE_MAGIC;
            return magic;
        }

        size_t GetTotalSize()
        {
            return Modulus.size() + Exponent.size() + Prime1.size() + Prime2.size() + Exponent1.size() + Exponent2.size()
                + Coefficient.size() + PrivateExponent.size();
        }

        static RsaKeyParams FromBlob(const std::vector<uint8_t>& blob)
        {
            RsaKeyParams result;
            const auto& header = *(reinterpret_cast<const BCRYPT_RSAKEY_BLOB*>(blob.data()));
            size_t offset = sizeof(BCRYPT_RSAKEY_BLOB);
            result.Exponent = ReadBytes(blob, offset, header.cbPublicExp);
            result.Modulus = ReadBytes(blob, offset, header.cbModulus);
            if (header.Magic == BCRYPT_RSAPRIVATE_MAGIC || header.Magic == BCRYPT_RSAFULLPRIVATE_MAGIC)
            {
                result.Prime1 = ReadBytes(blob, offset, header.cbPrime1);
                result.Prime2 = ReadBytes(blob, offset, header.cbPrime2);
            }
            if (header.Magic == BCRYPT_RSAFULLPRIVATE_MAGIC)
            {
                result.Exponent1 = ReadBytes(blob, offset, header.cbPrime1);
                result.Exponent2 = ReadBytes(blob, offset, header.cbPrime2);
                result.Coefficient = ReadBytes(blob, offset, header.cbPrime1);
                result.PrivateExponent = ReadBytes(blob, offset, header.cbModulus);
            }
            return result;
        }

        std::vector<uint8_t> ToBlob() const
        {
            auto magic = GetMagic();
            std::vector<uint8_t> blob(sizeof(BCRYPT_RSAKEY_BLOB));
            auto& header = *(reinterpret_cast<BCRYPT_RSAKEY_BLOB*>(blob.data()));
            header.Magic = magic;
            header.BitLength = static_cast<ULONG>(Modulus.size() * 8);
            header.cbPublicExp = static_cast<ULONG>(Exponent.size());
            header.cbModulus = static_cast<ULONG>(Modulus.size());
            header.cbPrime1 = static_cast<ULONG>(Prime1.size());
            header.cbPrime2 = static_cast<ULONG>(Prime2.size());

            WriteBytes(blob, Exponent);
            WriteBytes(blob, Modulus);
            if (magic == BCRYPT_RSAPRIVATE_MAGIC || magic == BCRYPT_RSAFULLPRIVATE_MAGIC)
            {
                WriteBytes(blob, Prime1);
                WriteBytes(blob, Prime2);
            }
            if (magic == BCRYPT_RSAFULLPRIVATE_MAGIC)
            {
                WriteBytes(blob, Exponent1);
                WriteBytes(blob, Exponent2);
                WriteBytes(blob, Coefficient);
                WriteBytes(blob, PrivateExponent);
            }
            return blob;
        }

    private:
        static std::vector<uint8_t> ReadBytes(const std::vector<uint8_t>& src, size_t& offset, size_t length)
        {
            std::vector<uint8_t> result;
            result.insert(result.end(), src.begin() + offset, src.begin() + offset + length);
            offset += length;
            return result;
        }

        static void WriteBytes(std::vector<uint8_t>& dst, const std::vector<uint8_t>& src)
        {
            dst.insert(dst.end(), src.begin(), src.end());
        }
    };

public:
    BCRYPT_KEY_HANDLE GetKeyHandle() const
    {
        return _hKey;
    }

    CngRsaKey()
    {
        auto status = BCryptOpenAlgorithmProvider(&_hAlg, BCRYPT_RSA_ALGORITHM, NULL, 0);
        CngThrowOnBadStatus("BCryptOpenAlgorithmProvider", status);
    }

    ~CngRsaKey()
    {
        BCryptDestroyKey(_hKey);
        BCryptCloseAlgorithmProvider(_hAlg, 0);

        _hKey = {};
        _hAlg = {};
    }

    void SetPrivate(std::string_view pem) override
    {
        auto der = ReadPEM(pem, SZ_PRIVATE_BEGIN_TOKEN, SZ_PRIVATE_END_TOKEN);
        DerReader derReader(der);
        RsaKeyParams params;
        derReader.ReadSequenceHeader();
        derReader.ReadInteger();
        params.Modulus = derReader.ReadInteger();
        params.Exponent = derReader.ReadInteger();
        params.PrivateExponent = derReader.ReadInteger();
        params.Prime1 = derReader.ReadInteger();
        params.Prime2 = derReader.ReadInteger();
        params.Exponent1 = derReader.ReadInteger();
        params.Exponent2 = derReader.ReadInteger();
        params.Coefficient = derReader.ReadInteger();
        ImportKey(params);
    }

    void SetPublic(std::string_view pem) override
    {
        auto der = ReadPEM(pem, SZ_PUBLIC_BEGIN_TOKEN, SZ_PUBLIC_END_TOKEN);
        DerReader derReader(der);
        RsaKeyParams params;
        derReader.ReadSequenceHeader();
        params.Modulus = derReader.ReadInteger();
        params.Exponent = derReader.ReadInteger();
        ImportKey(params);
    }

    std::string GetPrivate() override
    {
        auto params = ExportKey();
        DerWriter derWriter;
        derWriter.WriteInteger({ 0 });
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
        auto params = ExportKey();
        DerWriter derWriter;
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
        Reset();
        try
        {
            auto status = BCryptGenerateKeyPair(_hAlg, &_hKey, 1024, 0);
            CngThrowOnBadStatus("BCryptGenerateKeyPair", status);
            status = BCryptFinalizeKeyPair(_hKey, 0);
            CngThrowOnBadStatus("BCryptFinalizeKeyPair", status);
            _keyBlobType = BCRYPT_RSAFULLPRIVATE_BLOB;
        }
        catch (const std::exception&)
        {
            Reset();
            throw;
        }
    }

private:
    static constexpr const char* SZ_PUBLIC_BEGIN_TOKEN = "-----BEGIN RSA PUBLIC KEY-----";
    static constexpr const char* SZ_PUBLIC_END_TOKEN = "-----END RSA PUBLIC KEY-----";
    static constexpr const char* SZ_PRIVATE_BEGIN_TOKEN = "-----BEGIN RSA PRIVATE KEY-----";
    static constexpr const char* SZ_PRIVATE_END_TOKEN = "-----END RSA PRIVATE KEY-----";

    BCRYPT_KEY_HANDLE _hKey{};
    BCRYPT_KEY_HANDLE _hAlg{};
    LPCWSTR _keyBlobType{};

    void Reset()
    {
        BCryptDestroyKey(_hKey);
        _hKey = {};
    }

    void ImportKey(const RsaKeyParams& params)
    {
        Reset();
        auto blob = params.ToBlob();
        _keyBlobType = params.GetMagic() == BCRYPT_RSAFULLPRIVATE_MAGIC ? BCRYPT_RSAFULLPRIVATE_BLOB : BCRYPT_RSAPUBLIC_BLOB;
        auto status = BCryptImportKeyPair(_hAlg, NULL, _keyBlobType, &_hKey, blob.data(), static_cast<ULONG>(blob.size()), 0);
        CngThrowOnBadStatus("BCryptImportKeyPair", status);
    }

    RsaKeyParams ExportKey()
    {
        ULONG cbOutput{};
        auto status = BCryptExportKey(_hKey, NULL, _keyBlobType, NULL, 0, &cbOutput, 0);
        CngThrowOnBadStatus("BCryptExportKey", status);
        std::vector<uint8_t> blob(cbOutput);
        status = BCryptExportKey(_hKey, NULL, _keyBlobType, blob.data(), cbOutput, &cbOutput, 0);
        CngThrowOnBadStatus("BCryptExportKey", status);
        return RsaKeyParams::FromBlob(blob);
    }

    static std::vector<uint8_t> ReadPEM(std::string_view pem, std::string_view beginToken, std::string_view endToken)
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
        if (!CryptBinaryToStringA(input.data(), static_cast<DWORD>(input.size()), flags, NULL, &chString))
        {
            throw std::runtime_error("CryptBinaryToStringA failed");
        }
        std::string result(chString, 0);
        if (!CryptBinaryToStringA(input.data(), static_cast<DWORD>(input.size()), flags, result.data(), &chString))
        {
            throw std::runtime_error("CryptBinaryToStringA failed");
        }

        // CryptBinaryToStringA returns length that includes null terminator
        result.resize(result.size() - 1);

        return result;
    }

    static std::vector<uint8_t> DecodeBase64(std::string_view input)
    {
        DWORD cbBinary{};
        if (!CryptStringToBinaryA(
                input.data(), static_cast<DWORD>(input.size()), CRYPT_STRING_BASE64, NULL, &cbBinary, NULL, NULL))
        {
            throw std::runtime_error("CryptStringToBinaryA failed");
        }
        std::vector<uint8_t> result(cbBinary);
        if (!CryptStringToBinaryA(
                input.data(), static_cast<DWORD>(input.size()), CRYPT_STRING_BASE64, result.data(), &cbBinary, NULL, NULL))
        {
            throw std::runtime_error("CryptStringToBinaryA failed");
        }
        return result;
    }
};

class CngRsaAlgorithm final : public RsaAlgorithm
{
public:
    std::vector<uint8_t> SignData(const RsaKey& key, const void* data, size_t dataLen) override
    {
        auto hKey = static_cast<const CngRsaKey&>(key).GetKeyHandle();
        auto [cbHash, pbHash] = HashData(data, dataLen);
        auto [cbSignature, pbSignature] = std::tuple<DWORD, PBYTE>();
        try
        {
            BCRYPT_PKCS1_PADDING_INFO paddingInfo{ BCRYPT_SHA256_ALGORITHM };
            auto status = BCryptSignHash(hKey, &paddingInfo, pbHash, cbHash, NULL, 0, &cbSignature, BCRYPT_PAD_PKCS1);
            CngThrowOnBadStatus("BCryptSignHash", status);
            pbSignature = reinterpret_cast<PBYTE>(HeapAlloc(GetProcessHeap(), 0, cbSignature));
            ThrowBadAllocOnNull(pbSignature);
            status = BCryptSignHash(
                hKey, &paddingInfo, pbHash, cbHash, pbSignature, cbSignature, &cbSignature, BCRYPT_PAD_PKCS1);
            CngThrowOnBadStatus("BCryptSignHash", status);

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

    bool VerifyData(const RsaKey& key, const void* data, size_t dataLen, const void* sig, size_t sigLen) override
    {
        auto hKey = static_cast<const CngRsaKey&>(key).GetKeyHandle();
        auto [cbHash, pbHash] = HashData(data, dataLen);
        auto [cbSignature, pbSignature] = ToHeap(sig, sigLen);

        BCRYPT_PKCS1_PADDING_INFO paddingInfo{ BCRYPT_SHA256_ALGORITHM };
        auto status = BCryptVerifySignature(hKey, &paddingInfo, pbHash, cbHash, pbSignature, cbSignature, BCRYPT_PAD_PKCS1);
        HeapFree(GetProcessHeap(), 0, pbSignature);
        return status == ERROR_SUCCESS;
    }

private:
    static std::tuple<DWORD, PBYTE> HashData(const void* data, size_t dataLen)
    {
        auto hash = Crypt::SHA256(data, dataLen);
        return ToHeap(hash.data(), hash.size());
    }

    static std::tuple<DWORD, PBYTE> ToHeap(const void* data, size_t dataLen)
    {
        auto cbHash = static_cast<DWORD>(dataLen);
        auto pbHash = reinterpret_cast<PBYTE>(HeapAlloc(GetProcessHeap(), 0, dataLen));
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
} // namespace Crypt

#endif
