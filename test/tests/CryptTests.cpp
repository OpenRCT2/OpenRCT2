/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TestData.h"
#include "helpers/StringHelpers.hpp"

#include <gtest/gtest.h>
#include <openrct2/core/Crypt.h>
#include <openrct2/core/File.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/network/NetworkKey.h>
#include <string>

class CryptTests : public testing::Test
{
public:
    template<typename T> void AssertHash(std::string expected, T hash)
    {
        auto actual = StringToHex(hash);
        ASSERT_EQ(expected, actual);
    }

    template<typename T> std::string StringToHex(T input)
    {
        std::string result;
        result.reserve(input.size() * 2);
        for (auto b : input)
        {
            static_assert(sizeof(b) == 1);

            char buf[3];
            snprintf(buf, 3, "%02x", b);
            result.append(buf);
        }
        return result;
    }

    std::string GetTestPrivateKeyPath()
    {
        return Path::Combine(TestData::GetBasePath(), u8"keys", u8"Player.privkey");
    }

    std::string GetTestPublicKeyPath()
    {
        return Path::Combine(TestData::GetBasePath(), u8"keys", u8"Player-56f4afb74622a23bd2539ee701fe1b2c13d7e6ba.pubkey");
    }
};

TEST_F(CryptTests, SHA1_Basic)
{
    std::string input = "The quick brown fox jumped over the lazy dog.";
    auto result = Crypt::SHA1(input.data(), input.size());
    AssertHash("c0854fb9fb03c41cce3802cb0d220529e6eef94e", result);
}

TEST_F(CryptTests, SHA1_Multiple)
{
    std::string input[] = {
        "Merry-go-round 2 looks too intense for me",
        "This park is really clean and tidy",
        "This balloon from Balloon Stall 1 is really good value",
    };

    auto alg = Crypt::CreateSHA1();
    for (const auto& s : input)
    {
        alg->Update(s.data(), s.size());
    }
    auto result = alg->Finish();

    AssertHash("758a238d9a4748f80cc81f12be3885d5e45d34c2", result);
}

TEST_F(CryptTests, SHA1_WithClear)
{
    std::string inputA = "Merry-go-round 2 looks too intense for me";
    std::string inputB = "This park is really clean and tidy";

    auto alg = Crypt::CreateSHA1();
    alg->Update(inputA.data(), inputA.size());
    alg->Clear();
    alg->Update(inputB.data(), inputB.size());
    AssertHash("203986d57ebdbcdc8a45b7ee74c665960fd7ff48", alg->Finish());
}

TEST_F(CryptTests, SHA1_Many)
{
    auto alg = Crypt::CreateSHA1();

    // First digest
    std::string inputA[] = {
        "Merry-go-round 2 looks too intense for me",
        "This park is really clean and tidy",
        "This balloon from Balloon Stall 1 is really good value",
    };
    for (const auto& s : inputA)
    {
        alg->Update(s.data(), s.size());
    }
    AssertHash("758a238d9a4748f80cc81f12be3885d5e45d34c2", alg->Finish());

    // No need to clear after a finish

    // Second digest
    std::string inputB[] = {
        "This balloon from Balloon Stall 1 is really good value"
        "This park is really clean and tidy",
        "Merry-go-round 2 looks too intense for me",
    };
    for (const auto& s : inputB)
    {
        alg->Update(s.data(), s.size());
    }
    AssertHash("ac46948f97d69fa766706e932ce82562b4f73aa7", alg->Finish());
}

TEST_F(CryptTests, RSA_Basic)
{
    std::vector<uint8_t> data = { 0, 1, 2, 3, 4, 5, 6, 7 };

    auto file = File::ReadAllText(GetTestPrivateKeyPath());
    auto key = Crypt::CreateRSAKey();
    key->SetPrivate(std::string_view(file.data(), file.size()));

    auto rsa = Crypt::CreateRSA();
    auto signature = rsa->SignData(*key, data.data(), data.size());
    bool verified = rsa->VerifyData(*key, data.data(), data.size(), signature.data(), signature.size());
    ASSERT_TRUE(verified);
}

TEST_F(CryptTests, RSA_VerifyWithPublic)
{
    std::vector<uint8_t> data = { 7, 6, 5, 4, 3, 2, 1, 0 };

    auto privateFile = File::ReadAllText(GetTestPrivateKeyPath());
    auto privateKey = Crypt::CreateRSAKey();
    privateKey->SetPrivate(std::string_view(privateFile.data(), privateFile.size()));

    auto publicFile = File::ReadAllText(GetTestPublicKeyPath());
    auto publicKey = Crypt::CreateRSAKey();
    publicKey->SetPublic(std::string_view(publicFile.data(), publicFile.size()));

    auto rsa = Crypt::CreateRSA();
    auto signature = rsa->SignData(*privateKey, data.data(), data.size());
    bool verified = rsa->VerifyData(*publicKey, data.data(), data.size(), signature.data(), signature.size());
    ASSERT_TRUE(verified);
}

TEST_F(CryptTests, RSAKey_GetPublic)
{
    auto inPem = NormaliseLineEndings(File::ReadAllText(GetTestPublicKeyPath()));
    auto publicKey = Crypt::CreateRSAKey();
    publicKey->SetPublic(inPem);
    auto outPem = publicKey->GetPublic();
    ASSERT_EQ(inPem, outPem);
}

TEST_F(CryptTests, RSAKey_Generate)
{
    auto key = Crypt::CreateRSAKey();

    // Test generate twice, first checking if the PEMs contain expected strings
    key->Generate();
    auto privatePem1 = key->GetPrivate();
    auto publicPem1 = key->GetPublic();
    ASSERT_NE(privatePem1.find("RSA PRIVATE KEY"), std::string::npos);
    ASSERT_NE(publicPem1.find("RSA PUBLIC KEY"), std::string::npos);

    key->Generate();
    auto privatePem2 = key->GetPrivate();
    auto publicPem2 = key->GetPublic();
    ASSERT_NE(privatePem2.find("RSA PRIVATE KEY"), std::string::npos);
    ASSERT_NE(publicPem2.find("RSA PUBLIC KEY"), std::string::npos);

    // Now check that generate gives a different key each time
    ASSERT_STRNE(privatePem1.c_str(), privatePem2.c_str());
    ASSERT_STRNE(publicPem1.c_str(), publicPem2.c_str());
}
