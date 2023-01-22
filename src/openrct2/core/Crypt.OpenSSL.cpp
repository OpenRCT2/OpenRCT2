/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if !defined(DISABLE_NETWORK) && !defined(_WIN32)

#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#    include "Crypt.h"

#    include <openssl/evp.h>
#    include <openssl/pem.h>
#    include <stdexcept>
#    include <string>
#    include <vector>
namespace OpenRCT2
{
    using namespace Crypt;

    static void OpenSSLThrowOnBadStatus(std::string_view name, int status)
    {
        if (status != 1)
        {
            throw std::runtime_error(std::string(name) + " failed: " + std::to_string(status));
        }
    }

    static void OpenSSLInitialise()
    {
        static bool _opensslInitialised = false;
        if (!_opensslInitialised)
        {
            _opensslInitialised = true;
            OpenSSL_add_all_algorithms();
        }
    }

    template<typename TBase> class OpenSSLHashAlgorithm final : public TBase
    {
    private:
        const EVP_MD* _type;
        EVP_MD_CTX* _ctx{};
        bool _initialised{};

    public:
        OpenSSLHashAlgorithm(const EVP_MD* type)
        {
            _type = type;
            _ctx = EVP_MD_CTX_create();
            if (_ctx == nullptr)
            {
                throw std::runtime_error("EVP_MD_CTX_create failed");
            }
        }

        ~OpenSSLHashAlgorithm()
        {
            EVP_MD_CTX_destroy(_ctx);
        }

        TBase* Clear() override
        {
            if (EVP_DigestInit_ex(_ctx, _type, nullptr) <= 0)
            {
                throw std::runtime_error("EVP_DigestInit_ex failed");
            }
            _initialised = true;
            return this;
        }

        TBase* Update(const void* data, size_t dataLen) override
        {
            // Auto initialise
            if (!_initialised)
            {
                Clear();
            }

            if (EVP_DigestUpdate(_ctx, data, dataLen) <= 0)
            {
                throw std::runtime_error("EVP_DigestUpdate failed");
            }
            return this;
        }

        typename TBase::Result Finish() override
        {
            if (!_initialised)
            {
                throw std::runtime_error("No data to hash.");
            }
            _initialised = false;

            typename TBase::Result result;
            unsigned int digestSize{};
            if (EVP_DigestFinal(_ctx, result.data(), &digestSize) <= 0)
            {
                EVP_MD_CTX_destroy(_ctx);
                throw std::runtime_error("EVP_DigestFinal failed");
            }
            if (digestSize != result.size())
            {
                throw std::runtime_error("Expected digest size to equal " + std::to_string(result.size()));
            }
            return result;
        }
    };

    class OpenSSLRsaKey final : public RsaKey
    {
    public:
        EVP_PKEY* GetEvpKey() const
        {
            return _evpKey;
        }

        ~OpenSSLRsaKey()
        {
            EVP_PKEY_free(_evpKey);
        }

        void Generate() override
        {
            auto ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
            if (ctx == nullptr)
            {
                throw std::runtime_error("EVP_PKEY_CTX_new_id failed");
            }

            try
            {
                auto status = EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048);
                if (status == 0)
                {
                    throw std::runtime_error("EVP_PKEY_CTX_set_rsa_keygen_bits failed");
                }

                status = EVP_PKEY_keygen_init(ctx);
                OpenSSLThrowOnBadStatus("EVP_PKEY_keygen_init", status);

                EVP_PKEY* key{};
                status = EVP_PKEY_keygen(ctx, &key);
                OpenSSLThrowOnBadStatus("EVP_PKEY_keygen", status);

                EVP_PKEY_free(_evpKey);
                _evpKey = key;

                EVP_PKEY_CTX_free(ctx);
            }
            catch (const std::exception&)
            {
                EVP_PKEY_CTX_free(ctx);
                throw;
            }
        }

        void SetPrivate(std::string_view pem) override
        {
            SetKey(pem, true);
        }

        void SetPublic(std::string_view pem) override
        {
            SetKey(pem, false);
        }

        std::string GetPrivate() override
        {
            return GetKey(true);
        }

        std::string GetPublic() override
        {
            return GetKey(false);
        }

    private:
        EVP_PKEY* _evpKey{};

        void SetKey(std::string_view pem, bool isPrivate)
        {
            // Read PEM data via BIO buffer
            // HACK first parameter is not const on MINGW for some reason
            auto bio = BIO_new_mem_buf(static_cast<const void*>(pem.data()), static_cast<int>(pem.size()));
            if (bio == nullptr)
            {
                throw std::runtime_error("BIO_new_mem_buf failed");
            }
            auto rsa = isPrivate ? PEM_read_bio_RSAPrivateKey(bio, nullptr, nullptr, nullptr)
                                 : PEM_read_bio_RSAPublicKey(bio, nullptr, nullptr, nullptr);
            if (rsa == nullptr)
            {
                BIO_free_all(bio);
                auto msg = isPrivate ? "PEM_read_bio_RSAPrivateKey failed" : "PEM_read_bio_RSAPublicKey failed";
                throw std::runtime_error(msg);
            }
            BIO_free_all(bio);

            if (isPrivate && !RSA_check_key(rsa))
            {
                RSA_free(rsa);
                throw std::runtime_error("PEM key was invalid");
            }

            // Assign new key
            EVP_PKEY_free(_evpKey);
            _evpKey = EVP_PKEY_new();
            EVP_PKEY_set1_RSA(_evpKey, rsa);
            RSA_free(rsa);
        }

        std::string GetKey(bool isPrivate)
        {
            if (_evpKey == nullptr)
            {
                throw std::runtime_error("No key has been assigned");
            }

            auto rsa = EVP_PKEY_get1_RSA(_evpKey);
            if (rsa == nullptr)
            {
                throw std::runtime_error("EVP_PKEY_get1_RSA failed");
            }

            auto bio = BIO_new(BIO_s_mem());
            if (bio == nullptr)
            {
                throw std::runtime_error("BIO_new failed");
            }

            auto status = isPrivate ? PEM_write_bio_RSAPrivateKey(bio, rsa, nullptr, nullptr, 0, nullptr, nullptr)
                                    : PEM_write_bio_RSAPublicKey(bio, rsa);
            if (status != 1)
            {
                BIO_free_all(bio);
                RSA_free(rsa);
                throw std::runtime_error("PEM_write_bio_RSAPrivateKey failed");
            }
            RSA_free(rsa);

            auto keylen = BIO_pending(bio);
            std::string result(keylen, 0);
            BIO_read(bio, result.data(), keylen);
            BIO_free_all(bio);
            return result;
        }
    };

    class OpenSSLRsaAlgorithm final : public RsaAlgorithm
    {
    public:
        std::vector<uint8_t> SignData(const RsaKey& key, const void* data, size_t dataLen) override
        {
            auto evpKey = static_cast<const OpenSSLRsaKey&>(key).GetEvpKey();
            EVP_MD_CTX* mdctx{};
            try
            {
                mdctx = EVP_MD_CTX_create();
                if (mdctx == nullptr)
                {
                    throw std::runtime_error("EVP_MD_CTX_create failed");
                }

                auto status = EVP_DigestSignInit(mdctx, nullptr, EVP_sha256(), nullptr, evpKey);
                OpenSSLThrowOnBadStatus("EVP_DigestSignInit failed", status);

                status = EVP_DigestSignUpdate(mdctx, data, dataLen);
                OpenSSLThrowOnBadStatus("EVP_DigestSignUpdate failed", status);

                // Get required length of signature
                size_t sigLen{};
                status = EVP_DigestSignFinal(mdctx, nullptr, &sigLen);
                OpenSSLThrowOnBadStatus("EVP_DigestSignFinal failed", status);

                // Get signature
                std::vector<uint8_t> signature(sigLen);
                status = EVP_DigestSignFinal(mdctx, signature.data(), &sigLen);
                OpenSSLThrowOnBadStatus("EVP_DigestSignFinal failed", status);

                EVP_MD_CTX_destroy(mdctx);
                return signature;
            }
            catch (const std::exception&)
            {
                EVP_MD_CTX_destroy(mdctx);
                throw;
            }
        }

        bool VerifyData(const RsaKey& key, const void* data, size_t dataLen, const void* sig, size_t sigLen) override
        {
            auto evpKey = static_cast<const OpenSSLRsaKey&>(key).GetEvpKey();
            EVP_MD_CTX* mdctx{};
            try
            {
                mdctx = EVP_MD_CTX_create();
                if (mdctx == nullptr)
                {
                    throw std::runtime_error("EVP_MD_CTX_create failed");
                }

                auto status = EVP_DigestVerifyInit(mdctx, nullptr, EVP_sha256(), nullptr, evpKey);
                OpenSSLThrowOnBadStatus("EVP_DigestVerifyInit", status);

                status = EVP_DigestVerifyUpdate(mdctx, data, dataLen);
                OpenSSLThrowOnBadStatus("EVP_DigestVerifyUpdate", status);

                status = EVP_DigestVerifyFinal(mdctx, static_cast<const uint8_t*>(sig), sigLen);
                if (status != 0 && status != 1)
                {
                    OpenSSLThrowOnBadStatus("EVP_DigestVerifyUpdate", status);
                }
                EVP_MD_CTX_destroy(mdctx);
                return status == 1;
            }
            catch (const std::exception&)
            {
                EVP_MD_CTX_destroy(mdctx);
                throw;
            }
        }
    };

    namespace Crypt
    {
        std::unique_ptr<Sha1Algorithm> CreateSHA1()
        {
            OpenSSLInitialise();
            return std::make_unique<OpenSSLHashAlgorithm<Sha1Algorithm>>(EVP_sha1());
        }

        std::unique_ptr<Sha256Algorithm> CreateSHA256()
        {
            OpenSSLInitialise();
            return std::make_unique<OpenSSLHashAlgorithm<Sha256Algorithm>>(EVP_sha256());
        }

        std::unique_ptr<RsaAlgorithm> CreateRSA()
        {
            OpenSSLInitialise();
            return std::make_unique<OpenSSLRsaAlgorithm>();
        }

        std::unique_ptr<RsaKey> CreateRSAKey()
        {
            OpenSSLInitialise();
            return std::make_unique<OpenSSLRsaKey>();
        }
    } // namespace Crypt

#    pragma GCC diagnostic pop
} // namespace OpenRCT2
#endif // DISABLE_NETWORK
