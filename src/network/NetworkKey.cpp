#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "NetworkKey.h"
#include "../diagnostic.h"

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#define KEY_LENGTH_BITS 2048
#define KEY_TYPE EVP_PKEY_RSA

NetworkKey::NetworkKey()
{
    m_ctx = EVP_PKEY_CTX_new_id(KEY_TYPE, NULL);
    if (m_ctx == nullptr) {
        log_error("Failed to create OpenSSL context");
    }
}

NetworkKey::~NetworkKey()
{
    Unload();
    if (m_ctx != nullptr) {
        EVP_PKEY_CTX_free(m_ctx);
    }
}

void NetworkKey::Unload()
{
    if (m_key != nullptr) {
        EVP_PKEY_free(m_key);
        m_key = nullptr;
    }
}

bool NetworkKey::Generate()
{
    if (m_ctx == nullptr) {
        log_error("Invalid OpenSSL context");
        return false;
    }
#if KEY_TYPE == EVP_PKEY_RSA
    if (!EVP_PKEY_CTX_set_rsa_keygen_bits(m_ctx, KEY_LENGTH_BITS)) {
        log_error("Failed to set keygen params");
        EVP_PKEY_CTX_free(m_ctx);
        m_ctx = nullptr;
        return false;
    }
#else
    #error Only RSA is supported!
#endif
    if (!EVP_PKEY_keygen(m_ctx, &m_key)) {
        log_error("Failed to generate new key!");
        EVP_PKEY_CTX_free(m_ctx);
        m_ctx = nullptr;
        return false;
    }
    log_verbose("New key of type %d, length %d generated successfully.", KEY_TYPE, KEY_LENGTH_BITS);
    return true;
}

bool NetworkKey::LoadPrivate(SDL_RWops *file)
{
    int64_t size = file->size(file);
    if (size == -1) {
        log_error("unknown size, refusing to load key");
        return false;
    } else if (size > 4 * 1024 * 1024) {
        log_error("Key file suspiciously large, refusing to load it");
        return false;
    }
    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        log_error("Failed to initialise OpenSSL's BIO!");
        return false;
    }
    char *priv_key = new char[size];
    file->read(file, priv_key, 1, size);
    BIO_write(bio, priv_key, size);
    RSA *rsa;
    PEM_read_bio_RSAPrivateKey(bio, &rsa, nullptr, nullptr);
    if (!RSA_check_key(rsa)) {
        log_error("Loaded RSA key is invalid");
        BIO_free_all(bio);
        return false;
    }
    if (m_key != nullptr) {
        EVP_PKEY_free(m_key);
    }
    m_key = EVP_PKEY_new();
    EVP_PKEY_set1_RSA(m_key, rsa);
    BIO_free_all(bio);
    RSA_free(rsa);
    return true;
}

bool NetworkKey::LoadPublic(SDL_RWops *file)
{
    int64_t size = file->size(file);
    if (size == -1) {
        log_error("unknown size, refusing to load key");
        return false;
    } else if (size > 4 * 1024 * 1024) {
        log_error("Key file suspiciously large, refusing to load it");
        return false;
    }
    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        log_error("Failed to initialise OpenSSL's BIO!");
        return false;
    }
    char *priv_key = new char[size];
    file->read(file, priv_key, 1, size);
    BIO_write(bio, priv_key, size);
    RSA *rsa;
    PEM_read_bio_RSAPublicKey(bio, &rsa, nullptr, nullptr);
    if (m_key != nullptr) {
        EVP_PKEY_free(m_key);
    }
    m_key = EVP_PKEY_new();
    EVP_PKEY_set1_RSA(m_key, rsa);
    BIO_free_all(bio);
    RSA_free(rsa);
    return true;
}

bool NetworkKey::SavePrivate(SDL_RWops *file)
{
    if (m_key == nullptr) {
        log_error("No key loaded");
        return false;
    }
#if KEY_TYPE == EVP_PKEY_RSA
    RSA *rsa = EVP_PKEY_get1_RSA(m_key);
    if (rsa == nullptr) {
        log_error("Failed to get RSA key handle!");
        return false;
    }
    if (!RSA_check_key(rsa)) {
        log_error("Loaded RSA key is invalid");
        return false;
    }
    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        log_error("Failed to initialise OpenSSL's BIO!");
        return false;
    }
    int result = PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL);
    if (result != 1) {
        log_error("failed to write private key!");
        BIO_free_all(bio);
        return false;
    }
    RSA_free(rsa);

    int keylen = BIO_pending(bio);
    char *pem_key = new char[keylen];
    BIO_read(bio, pem_key, keylen);
    file->write(file, pem_key, keylen, 1);
    BIO_free_all(bio);
    delete [] pem_key;
#else
    #error Only RSA is supported!
#endif

    return true;
}

bool NetworkKey::SavePublic(SDL_RWops *file)
{
    if (m_key == nullptr) {
        log_error("No key loaded");
        return false;
    }
    RSA *rsa = EVP_PKEY_get1_RSA(m_key);
    if (rsa == nullptr) {
        log_error("Failed to get RSA key handle!");
        return false;
    }
    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        log_error("Failed to initialise OpenSSL's BIO!");
        return false;
    }
    int result = PEM_write_bio_RSAPublicKey(bio, rsa);
    if (result != 1) {
        log_error("failed to write private key!");
        BIO_free_all(bio);
        return false;
    }
    RSA_free(rsa);

    int keylen = BIO_pending(bio);
    char *pem_key = new char[keylen];
    BIO_read(bio, pem_key, keylen);
    file->write(file, pem_key, keylen, 1);
    BIO_free_all(bio);
    delete [] pem_key;

    return true;
}

char *NetworkKey::PublicKeyString()
{
    if (m_key == nullptr) {
        log_error("No key loaded");
        return nullptr;
    }
    RSA *rsa = EVP_PKEY_get1_RSA(m_key);
    if (rsa == nullptr) {
        log_error("Failed to get RSA key handle!");
        return nullptr;
    }
    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        log_error("Failed to initialise OpenSSL's BIO!");
        return nullptr;
    }
    int result = PEM_write_bio_RSAPublicKey(bio, rsa);
    if (result != 1) {
        log_error("failed to write private key!");
        BIO_free_all(bio);
        return nullptr;
    }
    RSA_free(rsa);

    int keylen = BIO_pending(bio);
    /* Use malloc here rather than new, so this pointer can be passed and
     * freed in C */
    char *pem_key = (char*)malloc(keylen + 1);
    BIO_read(bio, pem_key, keylen);
    BIO_free_all(bio);
    pem_key[keylen] = '\0';

    return pem_key;
}

bool NetworkKey::Sign(const char *md, const size_t len, char **signature, size_t *out_size)
{
    EVP_MD_CTX *mdctx = NULL;

    *signature = nullptr;

    /* Create the Message Digest Context */
    if (!(mdctx = EVP_MD_CTX_create())) {
        log_error("Failed to create MD context");
        return false;
    }
    /* Initialise the DigestSign operation - SHA-256 has been selected as the message digest function in this example */
    if (1 != EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, m_key)) {
        log_error("Failed to init digest sign");
        EVP_MD_CTX_destroy(mdctx);
        return false;
    }
    /* Call update with the message */
    if (1 != EVP_DigestSignUpdate(mdctx, md, len)) {
        log_error("Failed to goto update digest");
        EVP_MD_CTX_destroy(mdctx);
        return false;
    }

    /* Finalise the DigestSign operation */
    /* First call EVP_DigestSignFinal with a NULL sig parameter to obtain the length of the
     * signature. Length is returned in slen */
    if (1 != EVP_DigestSignFinal(mdctx, NULL, out_size)) {
        log_error("failed to finalise signature");
        EVP_MD_CTX_destroy(mdctx);
        return false;
    }

    unsigned char *sig;
    /* Allocate memory for the signature based on size in slen */
    if (!(sig = (unsigned char*)OPENSSL_malloc(sizeof(unsigned char) * (*out_size)))) {
        log_error("Failed to crypto-allocate space fo signature");
        EVP_MD_CTX_destroy(mdctx);
        return false;
    }
    /* Obtain the signature */
    if (1 != EVP_DigestSignFinal(mdctx, sig, out_size)) {
        log_error("Failed to finalise signature");
        EVP_MD_CTX_destroy(mdctx);
        OPENSSL_free(sig);
        return false;
    }
    *signature = new char[*out_size];
    memcpy(*signature, sig, *out_size);
    OPENSSL_free(sig);
    EVP_MD_CTX_destroy(mdctx);

    return true;
}

bool NetworkKey::Verify(const char *md, const size_t len, const char* sig, const size_t siglen)
{
    EVP_MD_CTX *mdctx = NULL;

    /* Create the Message Digest Context */
    if (!(mdctx = EVP_MD_CTX_create())) {
        log_error("Failed to create MD context");
        return false;
    }

    if (1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, m_key)) {
        log_error("Failed to initalise verification routine");
        EVP_MD_CTX_destroy(mdctx);
        return false;
    }

    /* Initialize `key` with a public key */
    if (1 != EVP_DigestVerifyUpdate(mdctx, md, len)) {
        log_error("Failed to update verification");
        EVP_MD_CTX_destroy(mdctx);
        return false;
    }

    if (1 == EVP_DigestVerifyFinal(mdctx, (unsigned char*)sig, siglen)) {
        EVP_MD_CTX_destroy(mdctx);
        log_verbose("Succesfully verified signature");
        return true;
    } else {
        EVP_MD_CTX_destroy(mdctx);
        log_error("Signature is invalid");
        return true;
    }
}
