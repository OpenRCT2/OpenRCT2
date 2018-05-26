#include <string>
#include <openrct2/core/Crypt.h>
#include <openrct2/core/File.h>
#include <openrct2/network/NetworkKey.h>
#include <gtest/gtest.h>

class CryptTests : public testing::Test
{
public:
    template<typename T>
    void AssertHash(std::string expected, T hash)
    {
        auto actual = StringToHex(hash);
        ASSERT_EQ(expected, actual);
    }

    template<typename T>
    std::string StringToHex(T input)
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
};

TEST_F(CryptTests, SHA1_Basic)
{
    std::string input = "The quick brown fox jumped over the lazy dog.";
    auto result = Hash::SHA1(input.data(), input.size());
    AssertHash("c0854fb9fb03c41cce3802cb0d220529e6eef94e", result);
}

TEST_F(CryptTests, SHA1_Multiple)
{
    std::string input[] = {
        "Merry-go-round 2 looks too intense for me",
        "This park is really clean and tidy",
        "This balloon from Balloon Stall 1 is really good value"
    };

    auto alg = Hash::CreateSHA1();
    for (auto s : input)
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

    auto alg = Hash::CreateSHA1();
    alg->Update(inputA.data(), inputA.size());
    alg->Clear();
    alg->Update(inputB.data(), inputB.size());
    AssertHash("203986d57ebdbcdc8a45b7ee74c665960fd7ff48", alg->Finish());
}

TEST_F(CryptTests, SHA1_Many)
{
    auto alg = Hash::CreateSHA1();

    // First digest
    std::string inputA[] = {
        "Merry-go-round 2 looks too intense for me",
        "This park is really clean and tidy",
        "This balloon from Balloon Stall 1 is really good value"
    };
    for (auto s : inputA)
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
    for (auto s : inputB)
    {
        alg->Update(s.data(), s.size());
    }
    AssertHash("ac46948f97d69fa766706e932ce82562b4f73aa7", alg->Finish());
}

TEST_F(CryptTests, RSA_Basic)
{
    std::vector<uint8> data = { 0, 1, 2, 3, 4, 5, 6, 7 };

    auto file = File::ReadAllBytes("C:/Users/Ted/Documents/OpenRCT2/keys/Ted.privkey");
    auto key = Hash::CreateRSAKey();
    key->SetPrivate(std::string_view((const char *)file.data(), file.size()));

    auto rsa = Hash::CreateRSA();
    auto signature = rsa->SignData(*key, data.data(), data.size());
    bool verified = rsa->VerifyData(*key, data.data(), data.size(), signature.data(), signature.size());
    ASSERT_TRUE(verified);
}

TEST_F(CryptTests, RSA_VerifyWithPublic)
{
    std::vector<uint8> data = { 7, 6, 5, 4, 3, 2, 1, 0 };

    auto privateFile = File::ReadAllBytes("C:/Users/Ted/Documents/OpenRCT2/keys/Ted.privkey");
    auto privateKey = Hash::CreateRSAKey();
    privateKey->SetPrivate(std::string_view((const char *)privateFile.data(), privateFile.size()));

    auto publicFile = File::ReadAllBytes("C:/Users/Ted/Documents/OpenRCT2/keys/Ted-b298a310905df8865788bdc864560c3d4c3ba562.pubkey");
    auto publicKey = Hash::CreateRSAKey();
    publicKey->SetPublic(std::string_view((const char *)publicFile.data(), publicFile.size()));

    auto rsa = Hash::CreateRSA();
    auto signature = rsa->SignData(*privateKey, data.data(), data.size());
    bool verified = rsa->VerifyData(*publicKey, data.data(), data.size(), signature.data(), signature.size());
    ASSERT_TRUE(verified);
}
